#include <node.h>
#include <v8.h>

extern "C" {
#	include "redcarpet/html.h"
#	include "redcarpet/markdown.h"
}

using namespace v8;
using namespace node;

struct redcarpet_renderopt {
	struct html_renderopt html;
	//VALUE self;
	//VALUE base_class;
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

class Markdown : public ObjectWrap
{
	public:
		Markdown();
		~Markdown();
		Handle<Value> create(const Arguments &args);
	private:
		void md_flags(Handle<Object> hash, unsigned int *enabled_extensions_p);
		struct sd_markdown *markdown;
};

void Markdown::md_flags(Handle<Object> hash, unsigned int *enabled_extensions_p)
{
	unsigned int extensions = 0;

	/**
	 * Markdown extensions -- all disabled by default
	 */
//	if (rb_hash_lookup(hash, CSTR2SYM("no_intra_emphasis")) == Qtrue)
//		extensions |= MKDEXT_NO_INTRA_EMPHASIS;
	if (hash->Get(String::NewSymbol("no_intra_emphasis"))->BooleanValue())
		extensions |= MKDEXT_NO_INTRA_EMPHASIS;

//	if (rb_hash_lookup(hash, CSTR2SYM("tables")) == Qtrue)
//		extensions |= MKDEXT_TABLES;
	if (hash->Get(String::NewSymbol("tables"))->BooleanValue())
		extensions |= MKDEXT_TABLES;

//	if (rb_hash_lookup(hash, CSTR2SYM("fenced_code_blocks")) == Qtrue)
//		extensions |= MKDEXT_FENCED_CODE;
	if (hash->Get(String::NewSymbol("fenced_code_blocks"))->BooleanValue())
		extensions |= MKDEXT_FENCED_CODE;

//	if (rb_hash_lookup(hash, CSTR2SYM("autolink")) == Qtrue)
//		extensions |= MKDEXT_AUTOLINK;
	if (hash->Get(String::NewSymbol("autolink"))->BooleanValue())
		extensions |= MKDEXT_AUTOLINK;

//	if (rb_hash_lookup(hash, CSTR2SYM("strikethrough")) == Qtrue)
//		extensions |= MKDEXT_STRIKETHROUGH;
	if (hash->Get(String::NewSymbol("strikethrough"))->BooleanValue())
		extensions |= MKDEXT_STRIKETHROUGH;

//	if (rb_hash_lookup(hash, CSTR2SYM("lax_html_blocks")) == Qtrue)
//		extensions |= MKDEXT_LAX_HTML_BLOCKS;
	if (hash->Get(String::NewSymbol("lax_html_blocks"))->BooleanValue())
		extensions |= MKDEXT_LAX_HTML_BLOCKS;

//	if (rb_hash_lookup(hash, CSTR2SYM("space_after_headers")) == Qtrue)
//		extensions |= MKDEXT_SPACE_HEADERS;
	if (hash->Get(String::NewSymbol("space_after_headers"))->BooleanValue())
		extensions |= MKDEXT_SPACE_HEADERS;

//	if (rb_hash_lookup(hash, CSTR2SYM("superscript")) == Qtrue)
//		extensions |= MKDEXT_SUPERSCRIPT;
	if (hash->Get(String::NewSymbol("superscript"))->BooleanValue())
		extensions |= MKDEXT_SUPERSCRIPT;

	*enabled_extensions_p = extensions;
}

Handle<Value> Markdown::create(const Arguments &args)
{
	HandleScope scope;
	Wrap(args.This());

//	VALUE rb_markdown, rb_rndr, hash;
	Local<Value> rb_markdown, rb_rndr;

	unsigned int extensions = 0;

	struct rb_redcarpet_rndr *rndr;

	int arg_len = args.Length();
	if (arg_len < 1 || arg_len > 2) {
		return VException("Usage: new Markdown(renderer, extensions={})");
	}
//	if (rb_scan_args(argc, argv, "11", &rb_rndr, &hash) == 2)
//		rb_redcarpet_md_flags(hash, &extensions);
	rb_rndr = args[0];
	if (arg_len == 2) {
		if (!args[1]->IsObject()) {
			return VException("Markdown: 2nd argument isn't an object");
		}
		Local<Object> hash = Object::Cast(*args[1]);
		md_flags(hash, &extensions);
	}

//	if (rb_obj_is_kind_of(rb_rndr, rb_cClass))
//		rb_rndr = rb_funcall(rb_rndr, rb_intern("new"), 0);
// TODO

//	if (!rb_obj_is_kind_of(rb_rndr, rb_cRenderBase))
//		rb_raise(rb_eTypeError, "Invalid Renderer instance given");
// TODO

//	Data_Get_Struct(rb_rndr, struct rb_redcarpet_rndr, rndr);
// TODO

//	markdown = sd_markdown_new(extensions, 16, &rndr->callbacks, &rndr->options);
//	if (!markdown)
//		rb_raise(rb_eRuntimeError, "Failed to create new Renderer class");
//	markdown = sd_markdown_new(extensions, 16, &rndr->callbacks, &rndr->options);
//	if (!markdown)
//		return VException("Failed to create new Renderer class");

//	rb_markdown = Data_Wrap_Struct(klass, NULL, rb_redcarpet_md__free, markdown);
//	rb_iv_set(rb_markdown, "@renderer", rb_rndr);
// TODO

	return Undefined();
}

/*Handle<Value> rb_redcarpet_md_render(const Arguments &args)
{
	VALUE rb_rndr;
	struct buf *output_buf;
	struct sd_markdown *markdown;

	Check_Type(text, T_STRING);

	rb_rndr = rb_iv_get(self, "@renderer");
	Data_Get_Struct(self, struct sd_markdown, markdown);

	if (rb_respond_to(rb_rndr, rb_intern("preprocess")))
		text = rb_funcall(rb_rndr, rb_intern("preprocess"), 1, text);
  if (NIL_P(text))
    return Qnil;

	/ * initialize buffers * /
	output_buf = bufnew(128);

	/ * render the magic * /
	sd_markdown_render(
		output_buf,
		RSTRING_PTR(text),
		RSTRING_LEN(text),
		markdown);

	/ * build the Ruby string * /
	text = redcarpet_str_new(output_buf->data, output_buf->size, rb_enc_get(text));

	bufrelease(output_buf);

	if (rb_respond_to(rb_rndr, rb_intern("postprocess")))
		text = rb_funcall(rb_rndr, rb_intern("postprocess"), 1, text);

	return text;
}*/

Markdown::Markdown()
{
	printf("new!\n");
	markdown = NULL;
}

Markdown::~Markdown()
{
	printf("free!\n");
	if (markdown != NULL)
		sd_markdown_free(markdown);
}

Handle<Value> markdown_new(const Arguments &args)
{
	assert(args.IsConstructCall());
	HandleScope scope;
	Markdown *md = new Markdown();
	md->create(args);
	return args.This();
}

void init(Handle<Object> target) {
	HandleScope scope;
	Local<FunctionTemplate> t = FunctionTemplate::New(markdown_new);
	t->InstanceTemplate()->SetInternalFieldCount(1);
	t->SetClassName(String::New("Markdown"));
	target->Set(String::NewSymbol("Markdown"), t->GetFunction());
//	NODE_SET_METHOD(target, "render", markdown_render);
}

NODE_MODULE(binding, init);
