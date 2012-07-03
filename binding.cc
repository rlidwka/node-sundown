#include <node.h>
#include <v8.h>
#include <string.h>

extern "C" {
#	include "sundown/html.h"
#	include "sundown/markdown.h"
}

using namespace v8;
using namespace node;

struct redcarpet_renderopt {
	struct html_renderopt html;
};

struct rb_redcarpet_rndr {
	struct sd_callbacks callbacks;
	struct redcarpet_renderopt options;
};

Handle<Value> VException(const char *msg)
{
	HandleScope scope;
	return ThrowException(Exception::Error(String::New(msg)));
}

void get_md_flags(Handle<Object> hash, unsigned int *enabled_extensions_p)
{
	unsigned int extensions = 0;

	/**
	 * Markdown extensions -- all disabled by default
	 */

	if (hash->Get(String::NewSymbol("no_intra_emphasis"))->BooleanValue())
		extensions |= MKDEXT_NO_INTRA_EMPHASIS;

	if (hash->Get(String::NewSymbol("tables"))->BooleanValue())
		extensions |= MKDEXT_TABLES;

	if (hash->Get(String::NewSymbol("fenced_code_blocks"))->BooleanValue())
		extensions |= MKDEXT_FENCED_CODE;

	if (hash->Get(String::NewSymbol("autolink"))->BooleanValue())
		extensions |= MKDEXT_AUTOLINK;

	if (hash->Get(String::NewSymbol("strikethrough"))->BooleanValue())
		extensions |= MKDEXT_STRIKETHROUGH;

	if (hash->Get(String::NewSymbol("lax_html_blocks"))->BooleanValue())
		extensions |= MKDEXT_LAX_HTML_BLOCKS;

	if (hash->Get(String::NewSymbol("space_after_headers"))->BooleanValue())
		extensions |= MKDEXT_SPACE_HEADERS;

	if (hash->Get(String::NewSymbol("superscript"))->BooleanValue())
		extensions |= MKDEXT_SUPERSCRIPT;

	*enabled_extensions_p = extensions;
}

void get_render_flags(Handle<Object> hash, unsigned int *enabled_extensions_p)
{
	unsigned int render_flags = 0;

	if (hash->Get(String::NewSymbol("escape_html"))->BooleanValue())
		render_flags |= HTML_ESCAPE;

	if (hash->Get(String::NewSymbol("filter_html"))->BooleanValue())
		render_flags |= HTML_SKIP_HTML;

	if (hash->Get(String::NewSymbol("no_images"))->BooleanValue())
		render_flags |= HTML_SKIP_IMAGES;

	if (hash->Get(String::NewSymbol("no_links"))->BooleanValue())
		render_flags |= HTML_SKIP_LINKS;

	if (hash->Get(String::NewSymbol("no_styles"))->BooleanValue())
		render_flags |= HTML_SKIP_STYLE;

	if (hash->Get(String::NewSymbol("safe_links_only"))->BooleanValue())
		render_flags |= HTML_SAFELINK;

	if (hash->Get(String::NewSymbol("with_toc_data"))->BooleanValue())
		render_flags |= HTML_TOC;

	if (hash->Get(String::NewSymbol("hard_wrap"))->BooleanValue())
		render_flags |= HTML_HARD_WRAP;

	if (hash->Get(String::NewSymbol("xhtml"))->BooleanValue())
		render_flags |= HTML_USE_XHTML;

	*enabled_extensions_p = render_flags;
}

Handle<Value> markdown_render(const Arguments &args)
{
	HandleScope scope;
	
	unsigned int extensions = 0;
	unsigned int render_flags = 0;
	struct rb_redcarpet_rndr rndr;
	struct sd_markdown *markdown;

	// parse arguments
	int arg_len = args.Length();
	if (arg_len < 1 || arg_len > 2) {
		return VException("Usage: Sundown.render(text, extensions={})");
	}

	// get md and render flags
	Local<String> text_to_render = args[0]->ToString();
	if (arg_len == 2) {
		if (!args[1]->IsObject()) {
			return VException("render: 2nd argument isn't an object");
		}
		Local<Object> hash = Object::Cast(*args[1]);
		get_md_flags(hash, &extensions);
		get_render_flags(hash, &render_flags);
	}

	// initialize renderer TODO
	sdhtml_renderer(&rndr.callbacks, (struct html_renderopt *)&rndr.options.html, render_flags);

	markdown = sd_markdown_new(extensions, 16, &rndr.callbacks, &rndr.options);
	if (!markdown)
		return VException("Failed to create new Renderer class");

	// now rendering:
	
	struct buf *output_buf;
	/* initialize buffers */
	output_buf = bufnew(1024);

	int size = text_to_render->Utf8Length();
	char* text = (char*)malloc(size+1);
	text_to_render->WriteUtf8(text, size);
	/* render the magic */
	sd_markdown_render(
		output_buf,
		(const unsigned char*)text,
		size,
		markdown);

	Local<String> result = String::New((const char*)output_buf->data, output_buf->size);

	bufrelease(output_buf);
	free(text);

	return scope.Close(result);
}

void init(Handle<Object> target) {
	HandleScope scope;
	NODE_SET_METHOD(target, "render", markdown_render);
}

NODE_MODULE(binding, init);
