#include <node.h>
#include <v8.h>
#include <string.h>

extern "C" {
#	include "redcarpet/html.h"
#	include "redcarpet/markdown.h"
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

Handle<Value> markdown_render(const Arguments &args)
{
	HandleScope scope;
	unsigned int extensions = 0;

	struct rb_redcarpet_rndr rndr;
	struct sd_markdown *markdown;
	sdhtml_renderer(&rndr.callbacks, (struct html_renderopt *)&rndr.options.html, 0);

//	markdown = sd_markdown_new(extensions, 16, &rndr->callbacks, &rndr->options);
//	if (!markdown)
//		rb_raise(rb_eRuntimeError, "Failed to create new Renderer class");
	markdown = sd_markdown_new(extensions, 16, &rndr.callbacks, &rndr.options);
	if (!markdown)
		return VException("Failed to create new Renderer class");

//	rb_markdown = Data_Wrap_Struct(klass, NULL, rb_redcarpet_md__free, markdown);
//	rb_iv_set(rb_markdown, "@renderer", rb_rndr);
// TODO
	
	struct buf *output_buf;
	/* initialize buffers */
	output_buf = bufnew(128);

	Local<String> tt = args[0]->ToString();
	char text[65536];
	tt->WriteUtf8(text, -1, 0, 0);
	/* render the magic */
	sd_markdown_render(
		output_buf,
		(const unsigned char*)text,
		strlen(text),
		markdown);

	Local<String> result = String::New((const char*)output_buf->data, output_buf->size);

	bufrelease(output_buf);

	return scope.Close(result);
}

void init(Handle<Object> target) {
	HandleScope scope;
	NODE_SET_METHOD(target, "render", markdown_render);
}

NODE_MODULE(binding, init);
