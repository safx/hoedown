/* document.h - generic markdown parser */

#ifndef HOEDOWN_DOCUMENT_H
#define HOEDOWN_DOCUMENT_H

#include "buffer.h"
#include "autolink.h"

#ifdef __cplusplus
extern "C" {
#endif


/*************
 * CONSTANTS *
 *************/

typedef enum hoedown_extensions {
	/* block-level extensions */
	HOEDOWN_EXT_TABLES = (1 << 0),
	HOEDOWN_EXT_FENCED_CODE = (1 << 1),
	HOEDOWN_EXT_FOOTNOTES = (1 << 2),

	/* span-level extensions */
	HOEDOWN_EXT_AUTOLINK = (1 << 3),
	HOEDOWN_EXT_STRIKETHROUGH = (1 << 4),
	HOEDOWN_EXT_UNDERLINE = (1 << 5),
	HOEDOWN_EXT_HIGHLIGHT = (1 << 6),
	HOEDOWN_EXT_QUOTE = (1 << 7),
	HOEDOWN_EXT_SUPERSCRIPT = (1 << 8),
	HOEDOWN_EXT_MATH = (1 << 9),

	/* other flags */
	HOEDOWN_EXT_NO_INTRA_EMPHASIS = (1 << 11),
	HOEDOWN_EXT_SPACE_HEADERS = (1 << 12),
	HOEDOWN_EXT_MATH_EXPLICIT = (1 << 13),

	/* negative flags */
	HOEDOWN_EXT_DISABLE_INDENTED_CODE = (1 << 14)
} hoedown_extensions;

#define HOEDOWN_EXT_BLOCK (\
	HOEDOWN_EXT_TABLES |\
	HOEDOWN_EXT_FENCED_CODE |\
	HOEDOWN_EXT_FOOTNOTES )

#define HOEDOWN_EXT_SPAN (\
	HOEDOWN_EXT_AUTOLINK |\
	HOEDOWN_EXT_STRIKETHROUGH |\
	HOEDOWN_EXT_UNDERLINE |\
	HOEDOWN_EXT_HIGHLIGHT |\
	HOEDOWN_EXT_QUOTE |\
	HOEDOWN_EXT_SUPERSCRIPT |\
	HOEDOWN_EXT_MATH )

#define HOEDOWN_EXT_FLAGS (\
	HOEDOWN_EXT_NO_INTRA_EMPHASIS |\
	HOEDOWN_EXT_SPACE_HEADERS |\
	HOEDOWN_EXT_MATH_EXPLICIT )

#define HOEDOWN_EXT_NEGATIVE (\
	HOEDOWN_EXT_DISABLE_INDENTED_CODE )

typedef enum hoedown_list_flags {
	HOEDOWN_LIST_ORDERED = (1 << 0),
	HOEDOWN_LI_BLOCK = (1 << 1)	/* <li> containing block data */
} hoedown_list_flags;

typedef enum hoedown_table_flags {
	HOEDOWN_TABLE_ALIGN_LEFT = 1,
	HOEDOWN_TABLE_ALIGN_RIGHT = 2,
	HOEDOWN_TABLE_ALIGN_CENTER = 3,
	HOEDOWN_TABLE_ALIGNMASK = 3,
	HOEDOWN_TABLE_HEADER = 4
} hoedown_table_flags;

typedef enum hoedown_autolink_type {
	HOEDOWN_AUTOLINK_NONE,		/* used internally when it is not an autolink*/
	HOEDOWN_AUTOLINK_NORMAL,	/* normal http/http/ftp/mailto/etc link */
	HOEDOWN_AUTOLINK_EMAIL		/* e-mail link without explit mailto: */
} hoedown_autolink_type;

typedef enum hoedown_block_type {
	HOEDOWN_NODE_DOCUMENT,
	HOEDOWN_NODE_PARAGRAPH,
	HOEDOWN_NODE_FOOTNOTE_LIST,
	HOEDOWN_NODE_TABLE,
	HOEDOWN_NODE_TABLE_BODY,
    HOEDOWN_NODE_BLOCKCODE,     /* not used */
	HOEDOWN_NODE_BLOCKQUOTE,
	HOEDOWN_NODE_LIST,
	HOEDOWN_NODE_BLOCK_LAST,
} hoedown_block_type;

typedef enum hoedown_span_type {
	HOEDOWN_NODE_HEADER = HOEDOWN_NODE_BLOCK_LAST,
	HOEDOWN_NODE_EMPH1,
	HOEDOWN_NODE_EMPH2,
	HOEDOWN_NODE_EMPH3,
	HOEDOWN_NODE_QUOTE,
	HOEDOWN_NODE_LINK,
	HOEDOWN_NODE_SUPERSCRIPT,
	HOEDOWN_NODE_LISTITEM,
	HOEDOWN_NODE_ATXHEADER,
	HOEDOWN_NODE_TABLE_HEADER,
	HOEDOWN_NODE_TABLE_ROW,
	HOEDOWN_NODE_TABLE_CELL,
	HOEDOWN_NODE_FOOTNOTE_DEF,
	HOEDOWN_NODE_SPAN_LAST
} hoedown_span_type;


/*********
 * TYPES *
 *********/
typedef void* hoedown_object;
struct hoedown_document;
typedef struct hoedown_document hoedown_document;

struct hoedown_renderer_data {
	void *opaque;
};
typedef struct hoedown_renderer_data hoedown_renderer_data;

/* hoedown_renderer - functions for rendering parsed data */
struct hoedown_renderer {
	/* state object */
	void *opaque;

	/* block level callbacks - NULL skips the block */
	void (*blockcode)(hoedown_object ob, const hoedown_buffer *text, const hoedown_buffer *lang, const hoedown_renderer_data *data);
	void (*blockquote)(hoedown_object ob, const hoedown_object content, const hoedown_renderer_data *data);
	void (*header)(hoedown_object ob, const hoedown_object content, int level, const hoedown_renderer_data *data);
	void (*hrule)(hoedown_object ob, const hoedown_renderer_data *data);
	void (*list)(hoedown_object ob, const hoedown_object content, hoedown_list_flags flags, const hoedown_renderer_data *data);
	void (*listitem)(hoedown_object ob, const hoedown_object content, hoedown_list_flags flags, const hoedown_renderer_data *data);
	void (*paragraph)(hoedown_object ob, const hoedown_object content, const hoedown_renderer_data *data);
	void (*table)(hoedown_object ob, const hoedown_object content, const hoedown_renderer_data *data);
	void (*table_header)(hoedown_object ob, const hoedown_object content, const hoedown_renderer_data *data);
	void (*table_body)(hoedown_object ob, const hoedown_object content, const hoedown_renderer_data *data);
	void (*table_row)(hoedown_object ob, const hoedown_object content, const hoedown_renderer_data *data);
	void (*table_cell)(hoedown_object ob, const hoedown_object content, hoedown_table_flags flags, const hoedown_renderer_data *data);
	void (*footnotes)(hoedown_object ob, const hoedown_object content, const hoedown_renderer_data *data);
	void (*footnote_def)(hoedown_object ob, const hoedown_object content, unsigned int num, const hoedown_renderer_data *data);
	void (*blockhtml)(hoedown_object ob, const hoedown_buffer *text, const hoedown_renderer_data *data);

	/* span level callbacks - NULL or return 0 prints the span verbatim */
	int (*autolink)(hoedown_object ob, const hoedown_object link, hoedown_autolink_type type, const hoedown_renderer_data *data);
	int (*codespan)(hoedown_object ob, const hoedown_buffer *text, const hoedown_renderer_data *data);
	int (*double_emphasis)(hoedown_object ob, const hoedown_object content, const hoedown_renderer_data *data);
	int (*emphasis)(hoedown_object ob, const hoedown_object content, const hoedown_renderer_data *data);
	int (*underline)(hoedown_object ob, const hoedown_object content, const hoedown_renderer_data *data);
	int (*highlight)(hoedown_object ob, const hoedown_object content, const hoedown_renderer_data *data);
	int (*quote)(hoedown_object ob, const hoedown_object content, const hoedown_renderer_data *data);
	int (*image)(hoedown_object ob, const hoedown_object link, const hoedown_buffer *title, const hoedown_buffer *alt, const hoedown_renderer_data *data);
	int (*linebreak)(hoedown_object ob, const hoedown_renderer_data *data);
	int (*link)(hoedown_object ob, const hoedown_object content, const hoedown_buffer *link, const hoedown_buffer *title, const hoedown_renderer_data *data);
	int (*triple_emphasis)(hoedown_object ob, const hoedown_object content, const hoedown_renderer_data *data);
	int (*strikethrough)(hoedown_object ob, const hoedown_object content, const hoedown_renderer_data *data);
	int (*superscript)(hoedown_object ob, const hoedown_object content, const hoedown_renderer_data *data);
	int (*footnote_ref)(hoedown_object ob, unsigned int num, const hoedown_renderer_data *data);
	int (*math)(hoedown_object ob, const hoedown_buffer *text, int displaymode, const hoedown_renderer_data *data);
	int (*raw_html)(hoedown_object ob, const hoedown_buffer *text, const hoedown_renderer_data *data);

	/* low level callbacks - NULL copies input directly into the output */
	void (*entity)(hoedown_object ob, const hoedown_buffer *text, const hoedown_renderer_data *data);
	void (*normal_text)(hoedown_object ob, const hoedown_buffer *text, const hoedown_renderer_data *data);

	/* miscellaneous callbacks */
	void (*doc_header)(hoedown_object ob, int inline_render, const hoedown_renderer_data *data);
	void (*doc_footer)(hoedown_object ob, int inline_render, const hoedown_renderer_data *data);

	/* object */
	hoedown_object (*span_new)(enum hoedown_span_type, const hoedown_renderer_data *data);
	hoedown_object (*block_new)(enum hoedown_block_type, const hoedown_renderer_data *data);
	void (*span_free)(hoedown_object, enum hoedown_span_type, const hoedown_renderer_data *data);
	void (*block_free)(hoedown_object, enum hoedown_block_type, const hoedown_renderer_data *data);
};
typedef struct hoedown_renderer hoedown_renderer;


/*************
 * FUNCTIONS *
 *************/

/* hoedown_document_new: allocate a new document processor instance */
hoedown_document *hoedown_document_new(
	const hoedown_renderer *renderer,
	hoedown_extensions extensions,
	size_t max_nesting
) __attribute__ ((malloc));

/* hoedown_document_render: render regular Markdown using the document processor */
void hoedown_document_render(hoedown_document *doc, hoedown_object ob, const uint8_t *data, size_t size);

/* hoedown_document_render_inline: render inline Markdown using the document processor */
void hoedown_document_render_inline(hoedown_document *doc, hoedown_object ob, const uint8_t *data, size_t size);

/* hoedown_document_free: deallocate a document processor instance */
void hoedown_document_free(hoedown_document *doc);


#ifdef __cplusplus
}
#endif

#endif /** HOEDOWN_DOCUMENT_H **/
