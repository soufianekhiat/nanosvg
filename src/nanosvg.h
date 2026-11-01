/*
 * ImWidgets: nanosvg at memononen/nanosvg 239e102, changed (marked "ImWidgets change" below): unitless gradient
 * coordinates in objectBoundingBox units are fractions of the box; radial focal points are stored relative to the
 * center, in radii, and default to the center; CSS colors (#rgba, #rrggbbaa, rgb()/rgba()/hsl()/hsla() in every
 * syntax, names in any case, currentColor, inherit; invalid colors keep the inherited one); opacities in percent;
 * paint-order short forms; stop offsets clamped and kept in order; the CSS cascade (presentation attributes, then
 * style sheet rules for *, element, .class and #id selectors by specificity, then the style attribute); gradient
 * attributes inherited through href (SVG 1.1, 13.2.2) and plain href (SVG 2), objectBoundingBox gradients mapped from
 * the unit space after their transform, fr focal radii, r = 0 painting the last stop; <text> and <tspan> runs as shapes
 * with a text (NSVGshape::text), content passed untrimmed; clip-path as the bounding rectangle of the clip path in image
 * space, nested clips intersected (NSVGshape::clipBounds); <image> as a shape outlined by its innermost clip path (else
 * its rectangle) with the image's reference and corners (NSVGshape::image).
 *
 * Copyright (c) 2013-14 Mikko Mononen memon@inside.org
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * The SVG parser is based on Anti-Grain Geometry 2.4 SVG example
 * Copyright (C) 2002-2004 Maxim Shemanarev (McSeem) (http://www.antigrain.com/)
 *
 * Arc calculation code based on canvg (https://code.google.com/p/canvg/)
 *
 * Bounding box calculation based on http://blog.hackers-cafe.net/2009/06/how-to-calculate-bezier-curves-bounding.html
 *
 */

#ifndef NANOSVG_H
#define NANOSVG_H

#ifndef NANOSVG_CPLUSPLUS
#ifdef __cplusplus
extern "C" {
#endif
#endif

// NanoSVG is a simple stupid single-header-file SVG parse. The output of the parser is a list of cubic bezier shapes.
//
// The library suits well for anything from rendering scalable icons in your editor application to prototyping a game.
//
// NanoSVG supports a wide range of SVG features, but something may be missing, feel free to create a pull request!
//
// The shapes in the SVG images are transformed by the viewBox and converted to specified units.
// That is, you should get the same looking data as your designed in your favorite app.
//
// NanoSVG can return the paths in few different units. For example if you want to render an image, you may choose
// to get the paths in pixels, or if you are feeding the data into a CNC-cutter, you may want to use millimeters.
//
// The units passed to NanoSVG should be one of: 'px', 'pt', 'pc' 'mm', 'cm', or 'in'.
// DPI (dots-per-inch) controls how the unit conversion is done.
//
// If you don't know or care about the units stuff, "px" and 96 should get you going.


/* Example Usage:
	// Load SVG
	NSVGimage* image;
	image = nsvgParseFromFile("test.svg", "px", 96);
	printf("size: %f x %f\n", image->width, image->height);
	// Use...
	for (NSVGshape *shape = image->shapes; shape != NULL; shape = shape->next) {
		for (NSVGpath *path = shape->paths; path != NULL; path = path->next) {
			for (int i = 0; i < path->npts-1; i += 3) {
				float* p = &path->pts[i*2];
				drawCubicBez(p[0],p[1], p[2],p[3], p[4],p[5], p[6],p[7]);
			}
		}
	}
	// Delete
	nsvgDelete(image);
*/

enum NSVGpaintType {
	NSVG_PAINT_UNDEF = -1,
	NSVG_PAINT_NONE = 0,
	NSVG_PAINT_COLOR = 1,
	NSVG_PAINT_LINEAR_GRADIENT = 2,
	NSVG_PAINT_RADIAL_GRADIENT = 3
};

enum NSVGspreadType {
	NSVG_SPREAD_PAD = 0,
	NSVG_SPREAD_REFLECT = 1,
	NSVG_SPREAD_REPEAT = 2
};

enum NSVGlineJoin {
	NSVG_JOIN_MITER = 0,
	NSVG_JOIN_ROUND = 1,
	NSVG_JOIN_BEVEL = 2
};

enum NSVGlineCap {
	NSVG_CAP_BUTT = 0,
	NSVG_CAP_ROUND = 1,
	NSVG_CAP_SQUARE = 2
};

enum NSVGfillRule {
	NSVG_FILLRULE_NONZERO = 0,
	NSVG_FILLRULE_EVENODD = 1
};

enum NSVGflags {
	NSVG_FLAGS_VISIBLE = 0x01
};

enum NSVGpaintOrder {
	NSVG_PAINT_FILL = 0x00,
	NSVG_PAINT_MARKERS = 0x01,
	NSVG_PAINT_STROKE = 0x02,
};

typedef struct NSVGgradientStop {
	unsigned int color;
	float offset;
} NSVGgradientStop;

typedef struct NSVGgradient {
	float xform[6];
	char spread;
	float fx, fy;
	float fr;		// ImWidgets change: the focal radius (SVG 2), in radii.
	int nstops;
	NSVGgradientStop stops[1];
} NSVGgradient;

typedef struct NSVGpaint {
	signed char type;
	union {
		unsigned int color;
		NSVGgradient* gradient;
	};
} NSVGpaint;

typedef struct NSVGpath
{
	float* pts;					// Cubic bezier points: x0,y0, [cpx1,cpx1,cpx2,cpy2,x1,y1], ...
	int npts;					// Total number of bezier points.
	char closed;				// Flag indicating if shapes should be treated as closed.
	float bounds[4];			// Tight bounding box of the shape [minx,miny,maxx,maxy].
	struct NSVGpath* next;		// Pointer to next path, or NULL if last element.
} NSVGpath;

#define NSVG_MAX_FILTERS 4		// ImWidgets change: nested filters kept per shape.

typedef struct NSVGshape
{
	char id[64];				// Optional 'id' attr of the shape or its group
	NSVGpaint fill;				// Fill paint
	NSVGpaint stroke;			// Stroke paint
	float opacity;				// Opacity of the shape.
	float strokeWidth;			// Stroke width (scaled).
	float strokeDashOffset;		// Stroke dash offset (scaled).
	float strokeDashArray[8];	// Stroke dash array (scaled).
	char strokeDashCount;		// Number of dash values in dash array.
	char strokeLineJoin;		// Stroke join type.
	char strokeLineCap;			// Stroke cap type.
	float miterLimit;			// Miter limit
	char fillRule;				// Fill rule, see NSVGfillRule.
    unsigned char paintOrder;	// Encoded paint order (3×2-bit fields) see NSVGpaintOrder
	unsigned char flags;		// Logical or of NSVG_FLAGS_* flags
	float bounds[4];			// Tight bounding box of the shape [minx,miny,maxx,maxy].
	char fillGradient[64];		// Optional 'id' of fill gradient
	char strokeGradient[64];	// Optional 'id' of stroke gradient
	float xform[6];				// Root transformation for fill/stroke gradient
	NSVGpath* paths;			// Linked list of paths in the image.
	struct NSVGshape* next;		// Pointer to next shape, or NULL if last element.
	// ImWidgets change: a run of <text> (UTF-8, whitespace collapsed; NULL for other shapes). Its path's points are
	// the run's origin on the baseline, then the origin plus the text's x and y units, transformed like path points.
	char* text;
	float fontSize;				// In the text's units.
	char textAnchor;			// NSVGtextAnchor.
	char textContinues;			// Starts where the previous run ends (no x of its own).
	// ImWidgets change: clip-path (with those of its ancestors) as a rectangle in image space: exact for axis-aligned
	// rectangles under translations and scales (PDF conversions clip vector content so), the bounds of other clips.
	char hasClip;
	float clipBounds[4];		// [minx,miny,maxx,maxy]
	// ImWidgets change: an <image>: its href (a data URI or a path; NULL for other shapes) and corners (top left, top
	// right, bottom left) in image space; its paths outline what shows of it (its innermost clip path, else its rectangle).
	char* image;
	float imageCorners[6];
	// ImWidgets change: masks and filters, applied by the caller. maskId: the innermost mask over the shape; maskOf:
	// the <mask> this shape is content of (not drawn itself). filterIds: the filters over the shape, outermost first;
	// filterGroups: which element set each (the shapes of one group share its number).
	char maskId[64];
	char maskOf[64];
	char patternOf[64];			// The <pattern> this shape is content of (in the pattern's space; not drawn itself).
	char filterIds[NSVG_MAX_FILTERS][64];
	int filterGroups[NSVG_MAX_FILTERS];
	int filterCount;
} NSVGshape;

enum NSVGtextAnchor {		// ImWidgets change
	NSVG_ANCHOR_START = 0,
	NSVG_ANCHOR_MIDDLE = 1,
	NSVG_ANCHOR_END = 2,
};

// ImWidgets change: <filter> primitives as written (interpreted by the caller). Lengths in the user space of the
// filtered element.
enum NSVGfilterPrimType {
	NSVG_FE_OTHER = 0, NSVG_FE_FLOOD, NSVG_FE_BLEND, NSVG_FE_COLOR_MATRIX, NSVG_FE_MORPHOLOGY, NSVG_FE_OFFSET,
	NSVG_FE_GAUSSIAN_BLUR, NSVG_FE_COMPOSITE,
};

typedef struct NSVGfilterPrim
{
	int type;					// NSVGfilterPrimType.
	char in[32], in2[32], result[32];
	char op[16];				// feMorphology and feComposite operator, feBlend mode, feColorMatrix type.
	float values[20];			// feColorMatrix.
	int nvalues;
	float dx, dy;				// feOffset.
	float stdDeviation;			// feGaussianBlur (x).
	float radius;				// feMorphology (x).
	float k[4];					// feComposite arithmetic.
	unsigned int floodColor;	// feFlood, with its opacity in alpha.
	struct NSVGfilterPrim* next;
} NSVGfilterPrim;

typedef struct NSVGfilter
{
	char id[64];
	NSVGfilterPrim* prims;
	struct NSVGfilter* next;
} NSVGfilter;

// ImWidgets change: a <pattern>'s attributes (its content: shapes with NSVGshape::patternOf). Attributes it does not
// set (NSVG_PATTERN_SET_*) and its content, when it has none, come from the pattern it references (href).
enum NSVGpatternSet {
	NSVG_PATTERN_SET_X = 1 << 0, NSVG_PATTERN_SET_Y = 1 << 1, NSVG_PATTERN_SET_WIDTH = 1 << 2, NSVG_PATTERN_SET_HEIGHT = 1 << 3,
	NSVG_PATTERN_SET_UNITS = 1 << 4, NSVG_PATTERN_SET_CONTENT_UNITS = 1 << 5, NSVG_PATTERN_SET_XFORM = 1 << 6,
};

typedef struct NSVGpattern
{
	char id[64];
	char href[64];
	float x, y, width, height;
	char userSpace;				// patternUnits="userSpaceOnUse" (else objectBoundingBox: fractions of the shape's bounds).
	char contentBox;			// patternContentUnits="objectBoundingBox".
	float xform[6];				// patternTransform.
	int set;					// NSVGpatternSet.
	struct NSVGpattern* next;
} NSVGpattern;

typedef struct NSVGimage
{
	float width;				// Width of the image.
	float height;				// Height of the image.
	NSVGshape* shapes;			// Linked list of shapes in the image.
	NSVGfilter* filters;		// ImWidgets change
	NSVGpattern* patterns;		// ImWidgets change
} NSVGimage;

// Parses SVG file from a file, returns SVG image as paths.
NSVGimage* nsvgParseFromFile(const char* filename, const char* units, float dpi);

// Parses SVG file from a null terminated string, returns SVG image as paths.
// Important note: changes the string.
NSVGimage* nsvgParse(char* input, const char* units, float dpi);

// Duplicates a path.
NSVGpath* nsvgDuplicatePath(NSVGpath* p);

// Deletes an image.
void nsvgDelete(NSVGimage* image);

#ifndef NANOSVG_CPLUSPLUS
#ifdef __cplusplus
}
#endif
#endif

#ifdef NANOSVG_IMPLEMENTATION

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define NSVG_PI (3.14159265358979323846264338327f)
#define NSVG_KAPPA90 (0.5522847493f)	// Length proportional to radius of a cubic bezier handle for 90deg arcs.

#define NSVG_ALIGN_MIN 0
#define NSVG_ALIGN_MID 1
#define NSVG_ALIGN_MAX 2
#define NSVG_ALIGN_NONE 0
#define NSVG_ALIGN_MEET 1
#define NSVG_ALIGN_SLICE 2

#define NSVG_NOTUSED(v) do { (void)(1 ? (void)0 : ( (void)(v) ) ); } while(0)
#define NSVG_RGB(r, g, b) (((unsigned int)r) | ((unsigned int)g << 8) | ((unsigned int)b << 16))

#ifdef _MSC_VER
	#pragma warning (disable: 4996) // Switch off security warnings
	#pragma warning (disable: 4100) // Switch off unreferenced formal parameter warnings
	#ifdef __cplusplus
	#define NSVG_INLINE inline
	#else
	#define NSVG_INLINE
	#endif
#else
	#define NSVG_INLINE inline
#endif


static int nsvg__isspace(char c)
{
	return strchr(" \t\n\v\f\r", c) != 0;
}

static int nsvg__isdigit(char c)
{
	return c >= '0' && c <= '9';
}

static NSVG_INLINE float nsvg__minf(float a, float b) { return a < b ? a : b; }
static NSVG_INLINE float nsvg__maxf(float a, float b) { return a > b ? a : b; }


// Simple XML parser

#define NSVG_XML_TAG 1
#define NSVG_XML_CONTENT 2
#define NSVG_XML_MAX_ATTRIBS 256

static void nsvg__parseContent(char* s,
							   void (*contentCb)(void* ud, const char* s),
							   void* ud)
{
	// ImWidgets change: untrimmed, text keeps its leading spaces (nsvg__content() trims styles).
	if (*s && contentCb)
		(*contentCb)(ud, s);
}

static void nsvg__parseElement(char* s,
							   void (*startelCb)(void* ud, const char* el, const char** attr),
							   void (*endelCb)(void* ud, const char* el),
							   void* ud)
{
	const char* attr[NSVG_XML_MAX_ATTRIBS];
	int nattr = 0;
	char* name;
	int start = 0;
	int end = 0;
	char quote;

	// Skip white space after the '<'
	while (*s && nsvg__isspace(*s)) s++;

	// Check if the tag is end tag
	if (*s == '/') {
		s++;
		end = 1;
	} else {
		start = 1;
	}

	// Skip comments, data and preprocessor stuff.
	if (!*s || *s == '?' || *s == '!')
		return;

	// Get tag name
	name = s;
	while (*s && !nsvg__isspace(*s)) s++;
	if (*s) { *s++ = '\0'; }

	// Get attribs
	while (!end && *s && nattr < NSVG_XML_MAX_ATTRIBS-3) {
		char* name = NULL;
		char* value = NULL;

		// Skip white space before the attrib name
		while (*s && nsvg__isspace(*s)) s++;
		if (!*s) break;
		if (*s == '/') {
			end = 1;
			break;
		}
		name = s;
		// Find end of the attrib name.
		while (*s && !nsvg__isspace(*s) && *s != '=') s++;
		if (*s) { *s++ = '\0'; }
		// Skip until the beginning of the value.
		while (*s && *s != '\"' && *s != '\'') s++;
		if (!*s) break;
		quote = *s;
		s++;
		// Store value and find the end of it.
		value = s;
		while (*s && *s != quote) s++;
		if (*s) { *s++ = '\0'; }

		// Store only well formed attributes
		if (name && value) {
			attr[nattr++] = name;
			attr[nattr++] = value;
		}
	}

	// List terminator
	attr[nattr++] = 0;
	attr[nattr++] = 0;

	// Call callbacks.
	if (start && startelCb)
		(*startelCb)(ud, name, attr);
	if (end && endelCb)
		(*endelCb)(ud, name);
}

int nsvg__parseXML(char* input,
				   void (*startelCb)(void* ud, const char* el, const char** attr),
				   void (*endelCb)(void* ud, const char* el),
				   void (*contentCb)(void* ud, const char* s),
				   void* ud)
{
	char* s = input;
	char* mark = s;
	int state = NSVG_XML_CONTENT;
	while (*s) {
		if (*s == '<' && state == NSVG_XML_CONTENT && (strncmp(s, "<![CDATA[", 9) == 0 || strncmp(s, "<!--", 4) == 0)) {
			// ImWidgets change: a CDATA section is content (style sheets are often wrapped in one), a comment is
			// skipped whole (both were read as a tag ending at their first '>', swallowing the rest).
			const int cdata = s[2] == '[';
			char* end = strstr(s + (cdata ? 9 : 4), cdata ? "]]>" : "-->");
			*s = '\0';
			nsvg__parseContent(mark, contentCb, ud);
			if (end == NULL)
				break;
			*end = '\0';
			if (cdata)
				nsvg__parseContent(s + 9, contentCb, ud);
			s = mark = end + 3;
		} else if (*s == '<' && state == NSVG_XML_CONTENT) {
			// Start of a tag
			*s++ = '\0';
			nsvg__parseContent(mark, contentCb, ud);
			mark = s;
			state = NSVG_XML_TAG;
		} else if (*s == '>' && state == NSVG_XML_TAG) {
			// Start of a content or new tag.
			*s++ = '\0';
			nsvg__parseElement(mark, startelCb, endelCb, ud);
			mark = s;
			state = NSVG_XML_CONTENT;
		} else {
			s++;
		}
	}

	return 1;
}


/* Simple SVG parser. */

#define NSVG_MAX_ATTR 128

enum NSVGgradientUnits {
	NSVG_USER_SPACE = 0,
	NSVG_OBJECT_SPACE = 1
};

#define NSVG_MAX_DASHES 8
#define NSVG_MAX_CLASSES 32

enum NSVGunits {
	NSVG_UNITS_USER,
	NSVG_UNITS_PX,
	NSVG_UNITS_PT,
	NSVG_UNITS_PC,
	NSVG_UNITS_MM,
	NSVG_UNITS_CM,
	NSVG_UNITS_IN,
	NSVG_UNITS_PERCENT,
	NSVG_UNITS_EM,
	NSVG_UNITS_EX
};

typedef struct NSVGcoordinate {
	float value;
	int units;
} NSVGcoordinate;

typedef struct NSVGlinearData {
	NSVGcoordinate x1, y1, x2, y2;
} NSVGlinearData;

typedef struct NSVGradialData {
	NSVGcoordinate cx, cy, r, fx, fy, fr;		// ImWidgets change: fr (SVG 2)
} NSVGradialData;

typedef struct NSVGgradientData
{
	char id[64];
	char ref[64];
	signed char type;
	union {
		NSVGlinearData linear;
		NSVGradialData radial;
	};
	char spread;
	char units;
	float xform[6];
	int nstops;
	NSVGgradientStop* stops;
	struct NSVGgradientData* next;
	int set;		// ImWidgets change: the attributes it sets (NSVG_SET_*); the others come from the gradients it references.
} NSVGgradientData;

enum NSVGgradientSet {		// ImWidgets change
	NSVG_SET_X1 = 1 << 0, NSVG_SET_Y1 = 1 << 1, NSVG_SET_X2 = 1 << 2, NSVG_SET_Y2 = 1 << 3,
	NSVG_SET_CX = 1 << 4, NSVG_SET_CY = 1 << 5, NSVG_SET_R = 1 << 6, NSVG_SET_FX = 1 << 7, NSVG_SET_FY = 1 << 8, NSVG_SET_FR = 1 << 9,
	NSVG_SET_UNITS = 1 << 10, NSVG_SET_SPREAD = 1 << 11, NSVG_SET_XFORM = 1 << 12,
};

typedef struct NSVGattrib
{
	char id[64];
	float xform[6];
	unsigned int fillColor;
	unsigned int strokeColor;
	float opacity;
	float fillOpacity;
	float strokeOpacity;
	char fillGradient[64];
	char strokeGradient[64];
	float strokeWidth;
	float strokeDashOffset;
	float strokeDashArray[NSVG_MAX_DASHES];
	int strokeDashCount;
	char strokeLineJoin;
	char strokeLineCap;
	float miterLimit;
	char fillRule;
	float fontSize;
	char textAnchor;		// ImWidgets change
	char hasClip;			// ImWidgets change: the clip rectangle in effect (NSVGshape::clipBounds), and a
	float clipBounds[4];	// clip-path of the element not applied yet (its transform may come after it).
	char clipPending[64];
	struct NSVGclipData* clipData;	// ImWidgets change: the innermost clip path, and the transform it was referenced with.
	float clipXform[6];
	unsigned int stopColor;
	float stopOpacity;
	float stopOffset;
	float fillColorAlpha, strokeColorAlpha, stopColorAlpha;		// ImWidgets change: the alpha of the colors themselves.
	unsigned int currentColor;									// ImWidgets change: the color property (currentColor).
	float currentColorAlpha;
	char hasFill;
	char hasStroke;
	char visible;
    unsigned char paintOrder;
	char maskId[64];		// ImWidgets change: NSVGshape::maskId and filterIds; filterOwn: this element set its last
	char filterIds[NSVG_MAX_FILTERS][64];		// filter (styles can set it more than once).
	int filterGroups[NSVG_MAX_FILTERS];
	int filterCount;
	char filterOwn;
} NSVGattrib;

typedef struct NSVGclipData		// ImWidgets change
{
	char id[64];
	float bounds[4];			// Union of its shapes' bounds, in the clip's user space.
	int empty;
	NSVGpath* paths;			// Its shapes' outlines, in the clip's user space (images are cut to them).
	struct NSVGclipData* next;
} NSVGclipData;

typedef struct NSVGstyleDeclaration
{
	char* className;
	char* propertiesText;
	struct NSVGstyleDeclaration* next;
} NSVGstyleDeclaration;

typedef struct NSVGparser
{
	NSVGattrib attr[NSVG_MAX_ATTR];
	int attrHead;
	float* pts;
	int npts;
	int cpts;
	NSVGpath* plist;
	NSVGimage* image;
	NSVGstyleDeclaration* styles;
	NSVGgradientData* gradients;
	NSVGshape* shapesTail;
	float viewMinx, viewMiny, viewWidth, viewHeight;
	int alignX, alignY, alignType;
	float dpi;
	char pathFlag;
	char defsFlag;
	char styleFlag;
	char cascaded;		// ImWidgets change: the element's styling is applied (nsvg__cascade()); its attribute loop reads geometry.
	// ImWidgets change: inside <text>, the run being read, its position and whether it continues the previous run.
	char textFlag, textSpace, textContinues;
	// ImWidgets change: inside <clipPath>, its bounds collected from its shapes (no shapes are made).
	struct NSVGclipData* clips;
	struct NSVGclipData* clipCurrent;
	// ImWidgets change: inside <mask>, its id (its shapes are made, tagged NSVGshape::maskOf); inside <filter>, the
	// filter its primitives go to; the last filter group number given.
	char maskCurrent[64];
	char patternCurrent[64];	// ImWidgets change: inside <pattern>, its id (its shapes are made, tagged NSVGshape::patternOf).
	NSVGfilter* filterCurrent;
	int filterGroupCount;
	char* textBuf;
	int textLen, textCap;
	float textX, textY;
} NSVGparser;

static void nsvg__xformIdentity(float* t)
{
	t[0] = 1.0f; t[1] = 0.0f;
	t[2] = 0.0f; t[3] = 1.0f;
	t[4] = 0.0f; t[5] = 0.0f;
}

static void nsvg__xformSetTranslation(float* t, float tx, float ty)
{
	t[0] = 1.0f; t[1] = 0.0f;
	t[2] = 0.0f; t[3] = 1.0f;
	t[4] = tx; t[5] = ty;
}

static void nsvg__xformSetScale(float* t, float sx, float sy)
{
	t[0] = sx; t[1] = 0.0f;
	t[2] = 0.0f; t[3] = sy;
	t[4] = 0.0f; t[5] = 0.0f;
}

static void nsvg__xformSetSkewX(float* t, float a)
{
	t[0] = 1.0f; t[1] = 0.0f;
	t[2] = tanf(a); t[3] = 1.0f;
	t[4] = 0.0f; t[5] = 0.0f;
}

static void nsvg__xformSetSkewY(float* t, float a)
{
	t[0] = 1.0f; t[1] = tanf(a);
	t[2] = 0.0f; t[3] = 1.0f;
	t[4] = 0.0f; t[5] = 0.0f;
}

static void nsvg__xformSetRotation(float* t, float a)
{
	float cs = cosf(a), sn = sinf(a);
	t[0] = cs; t[1] = sn;
	t[2] = -sn; t[3] = cs;
	t[4] = 0.0f; t[5] = 0.0f;
}

static void nsvg__xformMultiply(float* t, float* s)
{
	float t0 = t[0] * s[0] + t[1] * s[2];
	float t2 = t[2] * s[0] + t[3] * s[2];
	float t4 = t[4] * s[0] + t[5] * s[2] + s[4];
	t[1] = t[0] * s[1] + t[1] * s[3];
	t[3] = t[2] * s[1] + t[3] * s[3];
	t[5] = t[4] * s[1] + t[5] * s[3] + s[5];
	t[0] = t0;
	t[2] = t2;
	t[4] = t4;
}

static void nsvg__xformInverse(float* inv, float* t)
{
	double invdet, det = (double)t[0] * t[3] - (double)t[2] * t[1];
	if (det > -1e-6 && det < 1e-6) {
		nsvg__xformIdentity(t);
		return;
	}
	invdet = 1.0 / det;
	inv[0] = (float)(t[3] * invdet);
	inv[2] = (float)(-t[2] * invdet);
	inv[4] = (float)(((double)t[2] * t[5] - (double)t[3] * t[4]) * invdet);
	inv[1] = (float)(-t[1] * invdet);
	inv[3] = (float)(t[0] * invdet);
	inv[5] = (float)(((double)t[1] * t[4] - (double)t[0] * t[5]) * invdet);
}

static void nsvg__xformPremultiply(float* t, float* s)
{
	float s2[6];
	memcpy(s2, s, sizeof(float)*6);
	nsvg__xformMultiply(s2, t);
	memcpy(t, s2, sizeof(float)*6);
}

static void nsvg__xformPoint(float* dx, float* dy, float x, float y, float* t)
{
	*dx = x*t[0] + y*t[2] + t[4];
	*dy = x*t[1] + y*t[3] + t[5];
}

static void nsvg__xformVec(float* dx, float* dy, float x, float y, float* t)
{
	*dx = x*t[0] + y*t[2];
	*dy = x*t[1] + y*t[3];
}

#define NSVG_EPSILON (1e-12)

static int nsvg__ptInBounds(float* pt, float* bounds)
{
	return pt[0] >= bounds[0] && pt[0] <= bounds[2] && pt[1] >= bounds[1] && pt[1] <= bounds[3];
}


static double nsvg__evalBezier(double t, double p0, double p1, double p2, double p3)
{
	double it = 1.0-t;
	return it*it*it*p0 + 3.0*it*it*t*p1 + 3.0*it*t*t*p2 + t*t*t*p3;
}

static void nsvg__curveBounds(float* bounds, float* curve)
{
	int i, j, count;
	double roots[2], a, b, c, b2ac, t, v;
	float* v0 = &curve[0];
	float* v1 = &curve[2];
	float* v2 = &curve[4];
	float* v3 = &curve[6];

	// Start the bounding box by end points
	bounds[0] = nsvg__minf(v0[0], v3[0]);
	bounds[1] = nsvg__minf(v0[1], v3[1]);
	bounds[2] = nsvg__maxf(v0[0], v3[0]);
	bounds[3] = nsvg__maxf(v0[1], v3[1]);

	// Bezier curve fits inside the convex hull of it's control points.
	// If control points are inside the bounds, we're done.
	if (nsvg__ptInBounds(v1, bounds) && nsvg__ptInBounds(v2, bounds))
		return;

	// Add bezier curve inflection points in X and Y.
	for (i = 0; i < 2; i++) {
		a = -3.0 * v0[i] + 9.0 * v1[i] - 9.0 * v2[i] + 3.0 * v3[i];
		b = 6.0 * v0[i] - 12.0 * v1[i] + 6.0 * v2[i];
		c = 3.0 * v1[i] - 3.0 * v0[i];
		count = 0;
		if (fabs(a) < NSVG_EPSILON) {
			if (fabs(b) > NSVG_EPSILON) {
				t = -c / b;
				if (t > NSVG_EPSILON && t < 1.0-NSVG_EPSILON)
					roots[count++] = t;
			}
		} else {
			b2ac = b*b - 4.0*c*a;
			if (b2ac > NSVG_EPSILON) {
				t = (-b + sqrt(b2ac)) / (2.0 * a);
				if (t > NSVG_EPSILON && t < 1.0-NSVG_EPSILON)
					roots[count++] = t;
				t = (-b - sqrt(b2ac)) / (2.0 * a);
				if (t > NSVG_EPSILON && t < 1.0-NSVG_EPSILON)
					roots[count++] = t;
			}
		}
		for (j = 0; j < count; j++) {
			v = nsvg__evalBezier(roots[j], v0[i], v1[i], v2[i], v3[i]);
			bounds[0+i] = nsvg__minf(bounds[0+i], (float)v);
			bounds[2+i] = nsvg__maxf(bounds[2+i], (float)v);
		}
	}
}

static unsigned char nsvg__encodePaintOrder(enum NSVGpaintOrder a, enum NSVGpaintOrder b, enum NSVGpaintOrder c) {
    return (a & 0x03) | ((b & 0x03) << 2) | ((c & 0x03) << 4);
}

static NSVGparser* nsvg__createParser(void)
{
	NSVGparser* p;
	p = (NSVGparser*)malloc(sizeof(NSVGparser));
	if (p == NULL) goto error;
	memset(p, 0, sizeof(NSVGparser));

	p->image = (NSVGimage*)malloc(sizeof(NSVGimage));
	if (p->image == NULL) goto error;
	memset(p->image, 0, sizeof(NSVGimage));

	// Init style
	nsvg__xformIdentity(p->attr[0].xform);
	memset(p->attr[0].id, 0, sizeof p->attr[0].id);
	p->attr[0].fillColor = NSVG_RGB(0,0,0);
	p->attr[0].strokeColor = NSVG_RGB(0,0,0);
	p->attr[0].opacity = 1;
	p->attr[0].fillOpacity = 1;
	p->attr[0].strokeOpacity = 1;
	p->attr[0].stopOpacity = 1;
	p->attr[0].fillColorAlpha = p->attr[0].strokeColorAlpha = p->attr[0].stopColorAlpha = 1;	// ImWidgets change
	p->attr[0].currentColor = NSVG_RGB(0, 0, 0);
	p->attr[0].currentColorAlpha = 1;
	p->attr[0].strokeWidth = 1;
	p->attr[0].strokeLineJoin = NSVG_JOIN_MITER;
	p->attr[0].strokeLineCap = NSVG_CAP_BUTT;
	p->attr[0].miterLimit = 4;
	p->attr[0].fillRule = NSVG_FILLRULE_NONZERO;
	p->attr[0].hasFill = 1;
	p->attr[0].visible = 1;
    p->attr[0].paintOrder = nsvg__encodePaintOrder(NSVG_PAINT_FILL, NSVG_PAINT_STROKE, NSVG_PAINT_MARKERS);

	return p;

error:
	if (p) {
		if (p->image) free(p->image);
		free(p);
	}
	return NULL;
}
static void nsvg__deleteStyles(NSVGstyleDeclaration* style) {
	while (style) {
		NSVGstyleDeclaration* next = style->next;
		free(style->className);
		free(style->propertiesText);
		free(style);
		style = next;
	}
}

static void nsvg__deletePaths(NSVGpath* path)
{
	while (path) {
		NSVGpath *next = path->next;
		if (path->pts != NULL)
			free(path->pts);
		free(path);
		path = next;
	}
}

static void nsvg__deletePaint(NSVGpaint* paint)
{
	if (paint->type == NSVG_PAINT_LINEAR_GRADIENT || paint->type == NSVG_PAINT_RADIAL_GRADIENT)
		free(paint->gradient);
}

static void nsvg__deleteGradientData(NSVGgradientData* grad)
{
	NSVGgradientData* next;
	while (grad != NULL) {
		next = grad->next;
		free(grad->stops);
		free(grad);
		grad = next;
	}
}

static void nsvg__deleteParser(NSVGparser* p)
{
	if (p != NULL) {
		nsvg__deleteStyles(p->styles);
		nsvg__deletePaths(p->plist);
		nsvg__deleteGradientData(p->gradients);
		nsvgDelete(p->image);
		free(p->pts);
		free(p->textBuf);		// ImWidgets change
		while (p->clips != NULL) {		// ImWidgets change
			NSVGclipData* next = p->clips->next;
			nsvg__deletePaths(p->clips->paths);
			free(p->clips);
			p->clips = next;
		}
		free(p);
	}
}

static void nsvg__resetPath(NSVGparser* p)
{
	p->npts = 0;
}

static void nsvg__addPoint(NSVGparser* p, float x, float y)
{
	if (p->npts+1 > p->cpts) {
		p->cpts = p->cpts ? p->cpts*2 : 8;
		p->pts = (float*)realloc(p->pts, p->cpts*2*sizeof(float));
		if (!p->pts) return;
	}
	p->pts[p->npts*2+0] = x;
	p->pts[p->npts*2+1] = y;
	p->npts++;
}

static void nsvg__moveTo(NSVGparser* p, float x, float y)
{
	if (p->npts > 0) {
		p->pts[(p->npts-1)*2+0] = x;
		p->pts[(p->npts-1)*2+1] = y;
	} else {
		nsvg__addPoint(p, x, y);
	}
}

static void nsvg__lineTo(NSVGparser* p, float x, float y)
{
	float px,py, dx,dy;
	if (p->npts > 0) {
		px = p->pts[(p->npts-1)*2+0];
		py = p->pts[(p->npts-1)*2+1];
		dx = x - px;
		dy = y - py;
		nsvg__addPoint(p, px + dx/3.0f, py + dy/3.0f);
		nsvg__addPoint(p, x - dx/3.0f, y - dy/3.0f);
		nsvg__addPoint(p, x, y);
	}
}

static void nsvg__cubicBezTo(NSVGparser* p, float cpx1, float cpy1, float cpx2, float cpy2, float x, float y)
{
	if (p->npts > 0) {
		nsvg__addPoint(p, cpx1, cpy1);
		nsvg__addPoint(p, cpx2, cpy2);
		nsvg__addPoint(p, x, y);
	}
}

static NSVGattrib* nsvg__getAttr(NSVGparser* p)
{
	return &p->attr[p->attrHead];
}

static void nsvg__pushAttr(NSVGparser* p)
{
	if (p->attrHead < NSVG_MAX_ATTR-1) {
		p->attrHead++;
		memcpy(&p->attr[p->attrHead], &p->attr[p->attrHead-1], sizeof(NSVGattrib));
		p->attr[p->attrHead].filterOwn = 0;		// ImWidgets change
	}
}

static void nsvg__popAttr(NSVGparser* p)
{
	if (p->attrHead > 0)
		p->attrHead--;
}

static float nsvg__actualOrigX(NSVGparser* p)
{
	return p->viewMinx;
}

static float nsvg__actualOrigY(NSVGparser* p)
{
	return p->viewMiny;
}

static float nsvg__actualWidth(NSVGparser* p)
{
	return p->viewWidth;
}

static float nsvg__actualHeight(NSVGparser* p)
{
	return p->viewHeight;
}

static float nsvg__actualLength(NSVGparser* p)
{
	float w = nsvg__actualWidth(p), h = nsvg__actualHeight(p);
	return sqrtf(w*w + h*h) / sqrtf(2.0f);
}

static float nsvg__convertToPixels(NSVGparser* p, NSVGcoordinate c, float orig, float length)
{
	NSVGattrib* attr = nsvg__getAttr(p);
	switch (c.units) {
		case NSVG_UNITS_USER:		return c.value;
		case NSVG_UNITS_PX:			return c.value;
		case NSVG_UNITS_PT:			return c.value / 72.0f * p->dpi;
		case NSVG_UNITS_PC:			return c.value / 6.0f * p->dpi;
		case NSVG_UNITS_MM:			return c.value / 25.4f * p->dpi;
		case NSVG_UNITS_CM:			return c.value / 2.54f * p->dpi;
		case NSVG_UNITS_IN:			return c.value * p->dpi;
		case NSVG_UNITS_EM:			return c.value * attr->fontSize;
		case NSVG_UNITS_EX:			return c.value * attr->fontSize * 0.52f; // x-height of Helvetica.
		case NSVG_UNITS_PERCENT:	return orig + c.value / 100.0f * length;
		default:					return c.value;
	}
	return c.value;
}

static NSVGgradientData* nsvg__findGradientData(NSVGparser* p, const char* id)
{
	NSVGgradientData* grad = p->gradients;
	if (id == NULL || *id == '\0')
		return NULL;
	while (grad != NULL) {
		if (strcmp(grad->id, id) == 0)
			return grad;
		grad = grad->next;
	}
	return NULL;
}

// ImWidgets change: in objectBoundingBox units, unitless gradient coordinates are fractions of the box (SVG 1.1, 13.2).
static float nsvg__gradientToPixels(NSVGparser* p, NSVGcoordinate c, float orig, float length, int object)
{
	if (object && c.units == NSVG_UNITS_USER)
		return orig + c.value * length;
	return nsvg__convertToPixels(p, c, orig, length);
}

static NSVGgradient* nsvg__createGradient(NSVGparser* p, const char* id, const float* localBounds, float *xform, signed char* paintType)
{
	NSVGgradientData* data = NULL;
	NSVGgradientData* ref = NULL;
	NSVGgradientStop* stops = NULL;
	NSVGgradient* grad;
	float ox, oy, sw, sh, sl;
	int nstops = 0;
	int refIter;
	int zeroRadius = 0;		// ImWidgets change

	data = nsvg__findGradientData(p, id);
	if (data == NULL) return NULL;

	// TODO: use ref to fill in all unset values too.
	ref = data;
	refIter = 0;
	while (ref != NULL) {
		NSVGgradientData* nextRef = NULL;
		if (stops == NULL && ref->stops != NULL) {
			stops = ref->stops;
			nstops = ref->nstops;
			break;
		}
		nextRef = nsvg__findGradientData(p, ref->ref);
		if (nextRef == ref) break; // prevent infite loops on malformed data
		ref = nextRef;
		refIter++;
		if (refIter > 32) break; // prevent infite loops on malformed data
	}
	if (stops == NULL) return NULL;

	grad = (NSVGgradient*)malloc(sizeof(NSVGgradient) + sizeof(NSVGgradientStop)*(nstops-1));
	if (grad == NULL) return NULL;

	// ImWidgets change: the attributes it does not set come from the gradients it references (SVG 1.1, 13.2.2), its
	// geometry only from gradients of its type.
	{
		static NSVGgradientData resolved;
		NSVGgradientData* r = NULL;
		int iter = 0;
		resolved = *data;
		r = nsvg__findGradientData(p, data->ref);
		while (r != NULL && r != data && iter++ < 32) {
			const int geometry = r->type == resolved.type ? ~(NSVG_SET_UNITS | NSVG_SET_SPREAD | NSVG_SET_XFORM) : 0;
			const int want = ~resolved.set & r->set & (geometry | NSVG_SET_UNITS | NSVG_SET_SPREAD | NSVG_SET_XFORM);
			if (want & NSVG_SET_UNITS) resolved.units = r->units;
			if (want & NSVG_SET_SPREAD) resolved.spread = r->spread;
			if (want & NSVG_SET_XFORM) memcpy(resolved.xform, r->xform, sizeof(resolved.xform));
			if (resolved.type == NSVG_PAINT_LINEAR_GRADIENT) {
				if (want & NSVG_SET_X1) resolved.linear.x1 = r->linear.x1;
				if (want & NSVG_SET_Y1) resolved.linear.y1 = r->linear.y1;
				if (want & NSVG_SET_X2) resolved.linear.x2 = r->linear.x2;
				if (want & NSVG_SET_Y2) resolved.linear.y2 = r->linear.y2;
			} else {
				if (want & NSVG_SET_CX) resolved.radial.cx = r->radial.cx;
				if (want & NSVG_SET_CY) resolved.radial.cy = r->radial.cy;
				if (want & NSVG_SET_R) resolved.radial.r = r->radial.r;
				if (want & NSVG_SET_FX) resolved.radial.fx = r->radial.fx;
				if (want & NSVG_SET_FY) resolved.radial.fy = r->radial.fy;
				if (want & NSVG_SET_FR) resolved.radial.fr = r->radial.fr;
			}
			resolved.set |= want;
			r = nsvg__findGradientData(p, r->ref);
		}
		data = &resolved;
	}

	// The shape width and height.
	if (data->units == NSVG_OBJECT_SPACE) {
		// ImWidgets change: coordinates and gradientTransform in the box's unit space, then mapped onto the box (was
		// mapped first, so the transform turned around the user space's origin).
		ox = 0.0f;
		oy = 0.0f;
		sw = 1.0f;
		sh = 1.0f;
	} else {
		ox = nsvg__actualOrigX(p);
		oy = nsvg__actualOrigY(p);
		sw = nsvg__actualWidth(p);
		sh = nsvg__actualHeight(p);
	}
	sl = sqrtf(sw*sw + sh*sh) / sqrtf(2.0f);

	if (data->type == NSVG_PAINT_LINEAR_GRADIENT) {
		float x1, y1, x2, y2, dx, dy;
		x1 = nsvg__gradientToPixels(p, data->linear.x1, ox, sw, data->units == NSVG_OBJECT_SPACE);
		y1 = nsvg__gradientToPixels(p, data->linear.y1, oy, sh, data->units == NSVG_OBJECT_SPACE);
		x2 = nsvg__gradientToPixels(p, data->linear.x2, ox, sw, data->units == NSVG_OBJECT_SPACE);
		y2 = nsvg__gradientToPixels(p, data->linear.y2, oy, sh, data->units == NSVG_OBJECT_SPACE);
		// Calculate transform aligned to the line
		dx = x2 - x1;
		dy = y2 - y1;
		grad->xform[0] = dy; grad->xform[1] = -dx;
		grad->xform[2] = dx; grad->xform[3] = dy;
		grad->xform[4] = x1; grad->xform[5] = y1;
	} else {
		float cx, cy, fx, fy, r;
		cx = nsvg__gradientToPixels(p, data->radial.cx, ox, sw, data->units == NSVG_OBJECT_SPACE);
		cy = nsvg__gradientToPixels(p, data->radial.cy, oy, sh, data->units == NSVG_OBJECT_SPACE);
		// ImWidgets change: unset, fx and fy are cx and cy (SVG 1.1, 13.2.3).
		fx = (data->set & NSVG_SET_FX) ? nsvg__gradientToPixels(p, data->radial.fx, ox, sw, data->units == NSVG_OBJECT_SPACE) : cx;
		fy = (data->set & NSVG_SET_FY) ? nsvg__gradientToPixels(p, data->radial.fy, oy, sh, data->units == NSVG_OBJECT_SPACE) : cy;
		r = nsvg__gradientToPixels(p, data->radial.r, 0, sl, data->units == NSVG_OBJECT_SPACE);
		// Calculate transform aligned to the circle
		grad->xform[0] = r; grad->xform[1] = 0;
		grad->xform[2] = 0; grad->xform[3] = r;
		grad->xform[4] = cx; grad->xform[5] = cy;
		if (r <= 0.0f) {		// ImWidgets change: no radius paints the last stop's color (SVG 1.1, 13.2.3).
			r = 1.0f;
			zeroRadius = 1;
		}
		grad->xform[0] = r;
		grad->xform[3] = r;
		grad->fx = (fx - cx) / r;		// ImWidgets change: relative to the center (was fx / r).
		grad->fy = (fy - cy) / r;
		grad->fr = (data->set & NSVG_SET_FR) ? nsvg__gradientToPixels(p, data->radial.fr, 0, sl, data->units == NSVG_OBJECT_SPACE) / r : 0.0f;
		if (grad->fr < 0.0f) grad->fr = 0.0f;
	}

	nsvg__xformMultiply(grad->xform, data->xform);
	if (data->units == NSVG_OBJECT_SPACE) {		// ImWidgets change: unit space onto the box
		float box[6];
		box[0] = localBounds[2] - localBounds[0]; box[1] = 0.0f;
		box[2] = 0.0f; box[3] = localBounds[3] - localBounds[1];
		box[4] = localBounds[0]; box[5] = localBounds[1];
		nsvg__xformMultiply(grad->xform, box);
	}
	nsvg__xformMultiply(grad->xform, xform);

	grad->spread = data->spread;
	memcpy(grad->stops, stops, nstops*sizeof(NSVGgradientStop));
	grad->nstops = nstops;
	for (refIter = 0; zeroRadius && refIter < nstops; refIter++)		// ImWidgets change
		grad->stops[refIter] = grad->stops[nstops - 1];

	*paintType = data->type;

	return grad;
}

static float nsvg__getAverageScale(float* t)
{
	float sx = sqrtf(t[0]*t[0] + t[2]*t[2]);
	float sy = sqrtf(t[1]*t[1] + t[3]*t[3]);
	return (sx + sy) * 0.5f;
}

static void nsvg__getLocalBounds(float* bounds, NSVGshape *shape, float* xform)
{
	NSVGpath* path;
	float curve[4*2], curveBounds[4];
	int i, first = 1;
	for (path = shape->paths; path != NULL; path = path->next) {
		nsvg__xformPoint(&curve[0], &curve[1], path->pts[0], path->pts[1], xform);
		for (i = 0; i < path->npts-1; i += 3) {
			nsvg__xformPoint(&curve[2], &curve[3], path->pts[(i+1)*2], path->pts[(i+1)*2+1], xform);
			nsvg__xformPoint(&curve[4], &curve[5], path->pts[(i+2)*2], path->pts[(i+2)*2+1], xform);
			nsvg__xformPoint(&curve[6], &curve[7], path->pts[(i+3)*2], path->pts[(i+3)*2+1], xform);
			nsvg__curveBounds(curveBounds, curve);
			if (first) {
				bounds[0] = curveBounds[0];
				bounds[1] = curveBounds[1];
				bounds[2] = curveBounds[2];
				bounds[3] = curveBounds[3];
				first = 0;
			} else {
				bounds[0] = nsvg__minf(bounds[0], curveBounds[0]);
				bounds[1] = nsvg__minf(bounds[1], curveBounds[1]);
				bounds[2] = nsvg__maxf(bounds[2], curveBounds[2]);
				bounds[3] = nsvg__maxf(bounds[3], curveBounds[3]);
			}
			curve[0] = curve[6];
			curve[1] = curve[7];
		}
	}
}

// ImWidgets change: the element's pending clip-path, its clip's bounds through the element's transform, intersected
// with the clip in effect.
static void nsvg__applyClip(NSVGparser* p)
{
	NSVGattrib* attr = nsvg__getAttr(p);
	NSVGclipData* c;
	float b[4], x, y;
	int k;
	if (attr->clipPending[0] == '\0')
		return;
	for (c = p->clips; c != NULL && strcmp(c->id, attr->clipPending) != 0; c = c->next) {}
	attr->clipPending[0] = '\0';
	if (c == NULL)
		return;		// Unknown or later in the file: ignored.
	if (c->empty) {
		b[0] = b[1] = b[2] = b[3] = 0.0f;
	} else {
		for (k = 0; k < 4; k++) {
			nsvg__xformPoint(&x, &y, c->bounds[(k & 1) ? 2 : 0], c->bounds[(k & 2) ? 3 : 1], attr->xform);
			if (k == 0) { b[0] = b[2] = x; b[1] = b[3] = y; }
			else { b[0] = nsvg__minf(b[0], x); b[1] = nsvg__minf(b[1], y); b[2] = nsvg__maxf(b[2], x); b[3] = nsvg__maxf(b[3], y); }
		}
	}
	if (attr->hasClip) {
		b[0] = nsvg__maxf(b[0], attr->clipBounds[0]); b[1] = nsvg__maxf(b[1], attr->clipBounds[1]);
		b[2] = nsvg__minf(b[2], attr->clipBounds[2]); b[3] = nsvg__minf(b[3], attr->clipBounds[3]);
	}
	memcpy(attr->clipBounds, b, sizeof(b));
	attr->hasClip = 1;
	attr->clipData = c;
	memcpy(attr->clipXform, attr->xform, sizeof(attr->clipXform));
}

static void nsvg__deletePaths(NSVGpath* path);

static void nsvg__addShape(NSVGparser* p)
{
	NSVGattrib* attr = nsvg__getAttr(p);
	float scale = 1.0f;
	NSVGshape* shape;
	NSVGpath* path;
	int i;

	if (p->plist == NULL)
		return;
	if (p->clipCurrent != NULL) {		// ImWidgets change: a clip path's shape: its bounds only.
		NSVGclipData* c = p->clipCurrent;
		NSVGpath* last = NULL;
		for (path = p->plist; path != NULL; path = path->next) {
			if (c->empty) { memcpy(c->bounds, path->bounds, sizeof(c->bounds)); c->empty = 0; }
			else {
				c->bounds[0] = nsvg__minf(c->bounds[0], path->bounds[0]); c->bounds[1] = nsvg__minf(c->bounds[1], path->bounds[1]);
				c->bounds[2] = nsvg__maxf(c->bounds[2], path->bounds[2]); c->bounds[3] = nsvg__maxf(c->bounds[3], path->bounds[3]);
			}
			last = path;
		}
		if (last != NULL) {		// Kept: images are cut to them.
			last->next = c->paths;
			c->paths = p->plist;
		}
		p->plist = NULL;
		return;
	}
	nsvg__applyClip(p);		// ImWidgets change: the shape's own clip-path.

	shape = (NSVGshape*)malloc(sizeof(NSVGshape));
	if (shape == NULL) goto error;
	memset(shape, 0, sizeof(NSVGshape));

	memcpy(shape->id, attr->id, sizeof shape->id);
	memcpy(shape->fillGradient, attr->fillGradient, sizeof shape->fillGradient);
	memcpy(shape->strokeGradient, attr->strokeGradient, sizeof shape->strokeGradient);
	memcpy(shape->xform, attr->xform, sizeof shape->xform);
	scale = nsvg__getAverageScale(attr->xform);
	shape->strokeWidth = attr->strokeWidth * scale;
	shape->strokeDashOffset = attr->strokeDashOffset * scale;
	shape->strokeDashCount = (char)attr->strokeDashCount;
	for (i = 0; i < attr->strokeDashCount; i++)
		shape->strokeDashArray[i] = attr->strokeDashArray[i] * scale;
	shape->strokeLineJoin = attr->strokeLineJoin;
	shape->strokeLineCap = attr->strokeLineCap;
	shape->miterLimit = attr->miterLimit;
	shape->fillRule = attr->fillRule;
	shape->opacity = attr->opacity;
    shape->paintOrder = attr->paintOrder;

	shape->paths = p->plist;
	p->plist = NULL;

	// Calculate shape bounds
	shape->bounds[0] = shape->paths->bounds[0];
	shape->bounds[1] = shape->paths->bounds[1];
	shape->bounds[2] = shape->paths->bounds[2];
	shape->bounds[3] = shape->paths->bounds[3];
	for (path = shape->paths->next; path != NULL; path = path->next) {
		shape->bounds[0] = nsvg__minf(shape->bounds[0], path->bounds[0]);
		shape->bounds[1] = nsvg__minf(shape->bounds[1], path->bounds[1]);
		shape->bounds[2] = nsvg__maxf(shape->bounds[2], path->bounds[2]);
		shape->bounds[3] = nsvg__maxf(shape->bounds[3], path->bounds[3]);
	}

	// Set fill
	if (attr->hasFill == 0) {
		shape->fill.type = NSVG_PAINT_NONE;
	} else if (attr->hasFill == 1) {
		shape->fill.type = NSVG_PAINT_COLOR;
		shape->fill.color = attr->fillColor;
		shape->fill.color |= (unsigned int)(attr->fillOpacity*attr->fillColorAlpha*255) << 24;	// ImWidgets change: color alpha
	} else if (attr->hasFill == 2) {
		shape->fill.type = NSVG_PAINT_UNDEF;
	} else if (attr->hasFill == 3) {		// ImWidgets change: currentColor
		shape->fill.type = NSVG_PAINT_COLOR;
		shape->fill.color = attr->currentColor | ((unsigned int)(attr->fillOpacity*attr->currentColorAlpha*255) << 24);
	}

	// Set stroke
	if (attr->hasStroke == 0) {
		shape->stroke.type = NSVG_PAINT_NONE;
	} else if (attr->hasStroke == 1) {
		shape->stroke.type = NSVG_PAINT_COLOR;
		shape->stroke.color = attr->strokeColor;
		shape->stroke.color |= (unsigned int)(attr->strokeOpacity*attr->strokeColorAlpha*255) << 24;	// ImWidgets change: color alpha
	} else if (attr->hasStroke == 2) {
		shape->stroke.type = NSVG_PAINT_UNDEF;
	} else if (attr->hasStroke == 3) {		// ImWidgets change: currentColor
		shape->stroke.type = NSVG_PAINT_COLOR;
		shape->stroke.color = attr->currentColor | ((unsigned int)(attr->strokeOpacity*attr->currentColorAlpha*255) << 24);
	}

	// Set flags
	shape->flags = (attr->visible ? NSVG_FLAGS_VISIBLE : 0x00);
	shape->hasClip = attr->hasClip;		// ImWidgets change
	memcpy(shape->maskId, attr->maskId, sizeof(shape->maskId));
	memcpy(shape->maskOf, p->maskCurrent, sizeof(shape->maskOf));
	memcpy(shape->patternOf, p->patternCurrent, sizeof(shape->patternOf));
	memcpy(shape->filterIds, attr->filterIds, sizeof(shape->filterIds));
	memcpy(shape->filterGroups, attr->filterGroups, sizeof(shape->filterGroups));
	shape->filterCount = attr->filterCount;
	memcpy(shape->clipBounds, attr->clipBounds, sizeof(shape->clipBounds));

	// Add to tail
	if (p->image->shapes == NULL)
		p->image->shapes = shape;
	else
		p->shapesTail->next = shape;
	p->shapesTail = shape;

	return;

error:
	if (shape) free(shape);
}

static void nsvg__addPath(NSVGparser* p, char closed)
{
	NSVGattrib* attr = nsvg__getAttr(p);
	NSVGpath* path = NULL;
	float bounds[4];
	float* curve;
	int i;

	if (p->npts < 4)
		return;

	if (closed)
		nsvg__lineTo(p, p->pts[0], p->pts[1]);

	// Expect 1 + N*3 points (N = number of cubic bezier segments).
	if ((p->npts % 3) != 1)
		return;

	path = (NSVGpath*)malloc(sizeof(NSVGpath));
	if (path == NULL) goto error;
	memset(path, 0, sizeof(NSVGpath));

	path->pts = (float*)malloc(p->npts*2*sizeof(float));
	if (path->pts == NULL) goto error;
	path->closed = closed;
	path->npts = p->npts;

	// Transform path.
	for (i = 0; i < p->npts; ++i)
		nsvg__xformPoint(&path->pts[i*2], &path->pts[i*2+1], p->pts[i*2], p->pts[i*2+1], attr->xform);

	// Find bounds
	for (i = 0; i < path->npts-1; i += 3) {
		curve = &path->pts[i*2];
		nsvg__curveBounds(bounds, curve);
		if (i == 0) {
			path->bounds[0] = bounds[0];
			path->bounds[1] = bounds[1];
			path->bounds[2] = bounds[2];
			path->bounds[3] = bounds[3];
		} else {
			path->bounds[0] = nsvg__minf(path->bounds[0], bounds[0]);
			path->bounds[1] = nsvg__minf(path->bounds[1], bounds[1]);
			path->bounds[2] = nsvg__maxf(path->bounds[2], bounds[2]);
			path->bounds[3] = nsvg__maxf(path->bounds[3], bounds[3]);
		}
	}

	path->next = p->plist;
	p->plist = path;

	return;

error:
	if (path != NULL) {
		if (path->pts != NULL) free(path->pts);
		free(path);
	}
}

// We roll our own string to float because the std library one uses locale and messes things up.
static double nsvg__atof(const char* s)
{
	char* cur = (char*)s;
	char* end = NULL;
	double res = 0.0, sign = 1.0;
	long long intPart = 0, fracPart = 0;
	char hasIntPart = 0, hasFracPart = 0;

	// Parse optional sign
	if (*cur == '+') {
		cur++;
	} else if (*cur == '-') {
		sign = -1;
		cur++;
	}

	// Parse integer part
	if (nsvg__isdigit(*cur)) {
		// Parse digit sequence
		intPart = strtoll(cur, &end, 10);
		if (cur != end) {
			res = (double)intPart;
			hasIntPart = 1;
			cur = end;
		}
	}

	// Parse fractional part.
	if (*cur == '.') {
		cur++; // Skip '.'
		if (nsvg__isdigit(*cur)) {
			// Parse digit sequence
			fracPart = strtoll(cur, &end, 10);
			if (cur != end) {
				res += (double)fracPart / pow(10.0, (double)(end - cur));
				hasFracPart = 1;
				cur = end;
			}
		}
	}

	// A valid number should have integer or fractional part.
	if (!hasIntPart && !hasFracPart)
		return 0.0;

	// Parse optional exponent
	if (*cur == 'e' || *cur == 'E') {
		long expPart = 0;
		cur++; // skip 'E'
		expPart = strtol(cur, &end, 10); // Parse digit sequence with sign
		if (cur != end) {
			res *= pow(10.0, (double)expPart);
		}
	}

	return res * sign;
}


static const char* nsvg__parseNumber(const char* s, char* it, const int size)
{
	const int last = size-1;
	int i = 0;

	// sign
	if (*s == '-' || *s == '+') {
		if (i < last) it[i++] = *s;
		s++;
	}
	// integer part
	while (*s && nsvg__isdigit(*s)) {
		if (i < last) it[i++] = *s;
		s++;
	}
	if (*s == '.') {
		// decimal point
		if (i < last) it[i++] = *s;
		s++;
		// fraction part
		while (*s && nsvg__isdigit(*s)) {
			if (i < last) it[i++] = *s;
			s++;
		}
	}
	// exponent
	if ((*s == 'e' || *s == 'E') && (s[1] != 'm' && s[1] != 'x')) {
		if (i < last) it[i++] = *s;
		s++;
		if (*s == '-' || *s == '+') {
			if (i < last) it[i++] = *s;
			s++;
		}
		while (*s && nsvg__isdigit(*s)) {
			if (i < last) it[i++] = *s;
			s++;
		}
	}
	it[i] = '\0';

	return s;
}

static const char* nsvg__getNextPathItemWhenArcFlag(const char* s, char* it)
{
	it[0] = '\0';
	while (*s && (nsvg__isspace(*s) || *s == ',')) s++;
	if (!*s) return s;
	if (*s == '0' || *s == '1') {
		it[0] = *s++;
		it[1] = '\0';
		return s;
	}
	return s;
}

static const char* nsvg__getNextPathItem(const char* s, char* it)
{
	it[0] = '\0';
	// Skip white spaces and commas
	while (*s && (nsvg__isspace(*s) || *s == ',')) s++;
	if (!*s) return s;
	if (*s == '-' || *s == '+' || *s == '.' || nsvg__isdigit(*s)) {
		s = nsvg__parseNumber(s, it, 64);
	} else {
		// Parse command
		it[0] = *s++;
		it[1] = '\0';
		return s;
	}

	return s;
}

static unsigned int nsvg__parseColorHex(const char* str)
{
	unsigned int r=0, g=0, b=0;
	if (sscanf(str, "#%2x%2x%2x", &r, &g, &b) == 3 )		// 2 digit hex
		return NSVG_RGB(r, g, b);
	if (sscanf(str, "#%1x%1x%1x", &r, &g, &b) == 3 )		// 1 digit hex, e.g. #abc -> 0xccbbaa
		return NSVG_RGB(r*17, g*17, b*17);			// same effect as (r<<4|r), (g<<4|g), ..
	return NSVG_RGB(128, 128, 128);
}

// Parse rgb color. The pointer 'str' must point at "rgb(" (4+ characters).
// This function returns gray (rgb(128, 128, 128) == '#808080') on parse errors
// for backwards compatibility. Note: other image viewers return black instead.

static unsigned int nsvg__parseColorRGB(const char* str)
{
	int i;
	unsigned int rgbi[3];
	float rgbf[3];
	// try decimal integers first
	if (sscanf(str, "rgb(%u, %u, %u)", &rgbi[0], &rgbi[1], &rgbi[2]) != 3) {
		// integers failed, try percent values (float, locale independent)
		const char delimiter[3] = {',', ',', ')'};
		str += 4; // skip "rgb("
		for (i = 0; i < 3; i++) {
			while (*str && (nsvg__isspace(*str))) str++; 	// skip leading spaces
			if (*str == '+') str++;				// skip '+' (don't allow '-')
			if (!*str) break;
			rgbf[i] = nsvg__atof(str);

			// Note 1: it would be great if nsvg__atof() returned how many
			// bytes it consumed but it doesn't. We need to skip the number,
			// the '%' character, spaces, and the delimiter ',' or ')'.

			// Note 2: The following code does not allow values like "33.%",
			// i.e. a decimal point w/o fractional part, but this is consistent
			// with other image viewers, e.g. firefox, chrome, eog, gimp.

			while (*str && nsvg__isdigit(*str)) str++;		// skip integer part
			if (*str == '.') {
				str++;
				if (!nsvg__isdigit(*str)) break;		// error: no digit after '.'
				while (*str && nsvg__isdigit(*str)) str++;	// skip fractional part
			}
			if (*str == '%') str++; else break;
			while (*str && nsvg__isspace(*str)) str++;
			if (*str == delimiter[i]) str++;
			else break;
		}
		if (i == 3) {
			rgbi[0] = roundf(rgbf[0] * 2.55f);
			rgbi[1] = roundf(rgbf[1] * 2.55f);
			rgbi[2] = roundf(rgbf[2] * 2.55f);
		} else {
			rgbi[0] = rgbi[1] = rgbi[2] = 128;
		}
	}
	// clip values as the CSS spec requires
	for (i = 0; i < 3; i++) {
		if (rgbi[i] > 255) rgbi[i] = 255;
	}
	return NSVG_RGB(rgbi[0], rgbi[1], rgbi[2]);
}

typedef struct NSVGNamedColor {
	const char* name;
	unsigned int color;
} NSVGNamedColor;

NSVGNamedColor nsvg__colors[] = {

	{ "red", NSVG_RGB(255, 0, 0) },
	{ "green", NSVG_RGB( 0, 128, 0) },
	{ "blue", NSVG_RGB( 0, 0, 255) },
	{ "yellow", NSVG_RGB(255, 255, 0) },
	{ "cyan", NSVG_RGB( 0, 255, 255) },
	{ "magenta", NSVG_RGB(255, 0, 255) },
	{ "black", NSVG_RGB( 0, 0, 0) },
	{ "grey", NSVG_RGB(128, 128, 128) },
	{ "gray", NSVG_RGB(128, 128, 128) },
	{ "white", NSVG_RGB(255, 255, 255) },

#ifdef NANOSVG_ALL_COLOR_KEYWORDS
	{ "aliceblue", NSVG_RGB(240, 248, 255) },
	{ "antiquewhite", NSVG_RGB(250, 235, 215) },
	{ "aqua", NSVG_RGB( 0, 255, 255) },
	{ "aquamarine", NSVG_RGB(127, 255, 212) },
	{ "azure", NSVG_RGB(240, 255, 255) },
	{ "beige", NSVG_RGB(245, 245, 220) },
	{ "bisque", NSVG_RGB(255, 228, 196) },
	{ "blanchedalmond", NSVG_RGB(255, 235, 205) },
	{ "blueviolet", NSVG_RGB(138, 43, 226) },
	{ "brown", NSVG_RGB(165, 42, 42) },
	{ "burlywood", NSVG_RGB(222, 184, 135) },
	{ "cadetblue", NSVG_RGB( 95, 158, 160) },
	{ "chartreuse", NSVG_RGB(127, 255, 0) },
	{ "chocolate", NSVG_RGB(210, 105, 30) },
	{ "coral", NSVG_RGB(255, 127, 80) },
	{ "cornflowerblue", NSVG_RGB(100, 149, 237) },
	{ "cornsilk", NSVG_RGB(255, 248, 220) },
	{ "crimson", NSVG_RGB(220, 20, 60) },
	{ "darkblue", NSVG_RGB( 0, 0, 139) },
	{ "darkcyan", NSVG_RGB( 0, 139, 139) },
	{ "darkgoldenrod", NSVG_RGB(184, 134, 11) },
	{ "darkgray", NSVG_RGB(169, 169, 169) },
	{ "darkgreen", NSVG_RGB( 0, 100, 0) },
	{ "darkgrey", NSVG_RGB(169, 169, 169) },
	{ "darkkhaki", NSVG_RGB(189, 183, 107) },
	{ "darkmagenta", NSVG_RGB(139, 0, 139) },
	{ "darkolivegreen", NSVG_RGB( 85, 107, 47) },
	{ "darkorange", NSVG_RGB(255, 140, 0) },
	{ "darkorchid", NSVG_RGB(153, 50, 204) },
	{ "darkred", NSVG_RGB(139, 0, 0) },
	{ "darksalmon", NSVG_RGB(233, 150, 122) },
	{ "darkseagreen", NSVG_RGB(143, 188, 143) },
	{ "darkslateblue", NSVG_RGB( 72, 61, 139) },
	{ "darkslategray", NSVG_RGB( 47, 79, 79) },
	{ "darkslategrey", NSVG_RGB( 47, 79, 79) },
	{ "darkturquoise", NSVG_RGB( 0, 206, 209) },
	{ "darkviolet", NSVG_RGB(148, 0, 211) },
	{ "deeppink", NSVG_RGB(255, 20, 147) },
	{ "deepskyblue", NSVG_RGB( 0, 191, 255) },
	{ "dimgray", NSVG_RGB(105, 105, 105) },
	{ "dimgrey", NSVG_RGB(105, 105, 105) },
	{ "dodgerblue", NSVG_RGB( 30, 144, 255) },
	{ "firebrick", NSVG_RGB(178, 34, 34) },
	{ "floralwhite", NSVG_RGB(255, 250, 240) },
	{ "forestgreen", NSVG_RGB( 34, 139, 34) },
	{ "fuchsia", NSVG_RGB(255, 0, 255) },
	{ "gainsboro", NSVG_RGB(220, 220, 220) },
	{ "ghostwhite", NSVG_RGB(248, 248, 255) },
	{ "gold", NSVG_RGB(255, 215, 0) },
	{ "goldenrod", NSVG_RGB(218, 165, 32) },
	{ "greenyellow", NSVG_RGB(173, 255, 47) },
	{ "honeydew", NSVG_RGB(240, 255, 240) },
	{ "hotpink", NSVG_RGB(255, 105, 180) },
	{ "indianred", NSVG_RGB(205, 92, 92) },
	{ "indigo", NSVG_RGB( 75, 0, 130) },
	{ "ivory", NSVG_RGB(255, 255, 240) },
	{ "khaki", NSVG_RGB(240, 230, 140) },
	{ "lavender", NSVG_RGB(230, 230, 250) },
	{ "lavenderblush", NSVG_RGB(255, 240, 245) },
	{ "lawngreen", NSVG_RGB(124, 252, 0) },
	{ "lemonchiffon", NSVG_RGB(255, 250, 205) },
	{ "lightblue", NSVG_RGB(173, 216, 230) },
	{ "lightcoral", NSVG_RGB(240, 128, 128) },
	{ "lightcyan", NSVG_RGB(224, 255, 255) },
	{ "lightgoldenrodyellow", NSVG_RGB(250, 250, 210) },
	{ "lightgray", NSVG_RGB(211, 211, 211) },
	{ "lightgreen", NSVG_RGB(144, 238, 144) },
	{ "lightgrey", NSVG_RGB(211, 211, 211) },
	{ "lightpink", NSVG_RGB(255, 182, 193) },
	{ "lightsalmon", NSVG_RGB(255, 160, 122) },
	{ "lightseagreen", NSVG_RGB( 32, 178, 170) },
	{ "lightskyblue", NSVG_RGB(135, 206, 250) },
	{ "lightslategray", NSVG_RGB(119, 136, 153) },
	{ "lightslategrey", NSVG_RGB(119, 136, 153) },
	{ "lightsteelblue", NSVG_RGB(176, 196, 222) },
	{ "lightyellow", NSVG_RGB(255, 255, 224) },
	{ "lime", NSVG_RGB( 0, 255, 0) },
	{ "limegreen", NSVG_RGB( 50, 205, 50) },
	{ "linen", NSVG_RGB(250, 240, 230) },
	{ "maroon", NSVG_RGB(128, 0, 0) },
	{ "mediumaquamarine", NSVG_RGB(102, 205, 170) },
	{ "mediumblue", NSVG_RGB( 0, 0, 205) },
	{ "mediumorchid", NSVG_RGB(186, 85, 211) },
	{ "mediumpurple", NSVG_RGB(147, 112, 219) },
	{ "mediumseagreen", NSVG_RGB( 60, 179, 113) },
	{ "mediumslateblue", NSVG_RGB(123, 104, 238) },
	{ "mediumspringgreen", NSVG_RGB( 0, 250, 154) },
	{ "mediumturquoise", NSVG_RGB( 72, 209, 204) },
	{ "mediumvioletred", NSVG_RGB(199, 21, 133) },
	{ "midnightblue", NSVG_RGB( 25, 25, 112) },
	{ "mintcream", NSVG_RGB(245, 255, 250) },
	{ "mistyrose", NSVG_RGB(255, 228, 225) },
	{ "moccasin", NSVG_RGB(255, 228, 181) },
	{ "navajowhite", NSVG_RGB(255, 222, 173) },
	{ "navy", NSVG_RGB( 0, 0, 128) },
	{ "oldlace", NSVG_RGB(253, 245, 230) },
	{ "olive", NSVG_RGB(128, 128, 0) },
	{ "olivedrab", NSVG_RGB(107, 142, 35) },
	{ "orange", NSVG_RGB(255, 165, 0) },
	{ "orangered", NSVG_RGB(255, 69, 0) },
	{ "orchid", NSVG_RGB(218, 112, 214) },
	{ "palegoldenrod", NSVG_RGB(238, 232, 170) },
	{ "palegreen", NSVG_RGB(152, 251, 152) },
	{ "paleturquoise", NSVG_RGB(175, 238, 238) },
	{ "palevioletred", NSVG_RGB(219, 112, 147) },
	{ "papayawhip", NSVG_RGB(255, 239, 213) },
	{ "peachpuff", NSVG_RGB(255, 218, 185) },
	{ "peru", NSVG_RGB(205, 133, 63) },
	{ "pink", NSVG_RGB(255, 192, 203) },
	{ "plum", NSVG_RGB(221, 160, 221) },
	{ "powderblue", NSVG_RGB(176, 224, 230) },
	{ "purple", NSVG_RGB(128, 0, 128) },
	{ "rosybrown", NSVG_RGB(188, 143, 143) },
	{ "royalblue", NSVG_RGB( 65, 105, 225) },
	{ "saddlebrown", NSVG_RGB(139, 69, 19) },
	{ "salmon", NSVG_RGB(250, 128, 114) },
	{ "sandybrown", NSVG_RGB(244, 164, 96) },
	{ "seagreen", NSVG_RGB( 46, 139, 87) },
	{ "seashell", NSVG_RGB(255, 245, 238) },
	{ "sienna", NSVG_RGB(160, 82, 45) },
	{ "silver", NSVG_RGB(192, 192, 192) },
	{ "skyblue", NSVG_RGB(135, 206, 235) },
	{ "slateblue", NSVG_RGB(106, 90, 205) },
	{ "slategray", NSVG_RGB(112, 128, 144) },
	{ "slategrey", NSVG_RGB(112, 128, 144) },
	{ "snow", NSVG_RGB(255, 250, 250) },
	{ "springgreen", NSVG_RGB( 0, 255, 127) },
	{ "steelblue", NSVG_RGB( 70, 130, 180) },
	{ "tan", NSVG_RGB(210, 180, 140) },
	{ "teal", NSVG_RGB( 0, 128, 128) },
	{ "thistle", NSVG_RGB(216, 191, 216) },
	{ "tomato", NSVG_RGB(255, 99, 71) },
	{ "turquoise", NSVG_RGB( 64, 224, 208) },
	{ "violet", NSVG_RGB(238, 130, 238) },
	{ "wheat", NSVG_RGB(245, 222, 179) },
	{ "whitesmoke", NSVG_RGB(245, 245, 245) },
	{ "yellowgreen", NSVG_RGB(154, 205, 50) },
#endif
};

static unsigned int nsvg__parseColorName(const char* str)
{
	int i, ncolors = sizeof(nsvg__colors) / sizeof(NSVGNamedColor);

	for (i = 0; i < ncolors; i++) {
		if (strcmp(nsvg__colors[i].name, str) == 0) {
			return nsvg__colors[i].color;
		}
	}

	return NSVG_RGB(128, 128, 128);
}

static unsigned int nsvg__parseColor(const char* str)
{
	size_t len = 0;
	while(*str == ' ') ++str;
	len = strlen(str);
	if (len >= 1 && *str == '#')
		return nsvg__parseColorHex(str);
	else if (len >= 4 && str[0] == 'r' && str[1] == 'g' && str[2] == 'b' && str[3] == '(')
		return nsvg__parseColorRGB(str);
	return nsvg__parseColorName(str);
}

// ImWidgets change: CSS colors. 1 with *rgb (NSVG_RGB) and *alpha (0 to 1); 0 when invalid (the caller keeps the
// inherited color).
static int nsvg__cssNumber(const char** str, double* out, char* unit)
{
	const char* s = *str;
	const char* start;
	while (*s && nsvg__isspace(*s)) s++;
	start = s;
	if (*s == '+' || *s == '-') s++;
	if (!nsvg__isdigit(*s) && !(*s == '.' && nsvg__isdigit(s[1]))) return 0;
	while (nsvg__isdigit(*s)) s++;
	if (*s == '.') { s++; while (nsvg__isdigit(*s)) s++; }
	if ((*s == 'e' || *s == 'E') && (nsvg__isdigit(s[1]) || ((s[1] == '+' || s[1] == '-') && nsvg__isdigit(s[2])))) {
		s += 2;
		while (nsvg__isdigit(*s)) s++;
	}
	*out = nsvg__atof(start);
	*unit = 0;
	if (*s == '%') { *unit = '%'; s++; }
	else if (strncmp(s, "deg", 3) == 0) { *unit = 'd'; s += 3; }
	*str = s;
	return 1;
}

static float nsvg__hue(float m1, float m2, float h)
{
	if (h < 0) h += 1;
	if (h > 1) h -= 1;
	if (h < 1.0f / 6.0f) return m1 + (m2 - m1) * h * 6.0f;
	if (h < 0.5f) return m2;
	if (h < 2.0f / 3.0f) return m1 + (m2 - m1) * (2.0f / 3.0f - h) * 6.0f;
	return m1;
}

static int nsvg__isCurrentColor(const char* str)
{
	const char* k = "currentcolor";
	while (*str && nsvg__isspace(*str)) str++;
	while (*k && (*str == *k || *str == *k - 32)) { str++; k++; }
	while (*str && nsvg__isspace(*str)) str++;
	return *k == 0 && *str == 0;
}

static int nsvg__parseColorCss(NSVGparser* p, const char* str, unsigned int* rgb, float* alpha)
{
	char low[256];
	size_t n = 0, i;
	while (*str && nsvg__isspace(*str)) str++;
	while (str[n] && n < sizeof(low) - 1) { low[n] = (char)((str[n] >= 'A' && str[n] <= 'Z') ? str[n] + 32 : str[n]); n++; }
	low[n] = 0;
	while (n > 0 && nsvg__isspace(low[n - 1])) low[--n] = 0;
	*alpha = 1.0f;
	if (low[0] == '#') {
		unsigned int v[8];
		size_t len = n - 1;
		if (len != 3 && len != 4 && len != 6 && len != 8) return 0;
		for (i = 0; i < len; i++) {
			const char c = low[1 + i];
			v[i] = (c >= '0' && c <= '9') ? (unsigned int)(c - '0') : (c >= 'a' && c <= 'f') ? (unsigned int)(c - 'a' + 10) : 99u;
			if (v[i] > 15) return 0;
		}
		if (len <= 4) {
			*rgb = NSVG_RGB(v[0] * 17, v[1] * 17, v[2] * 17);
			if (len == 4) *alpha = (float)(v[3] * 17) / 255.0f;
		} else {
			*rgb = NSVG_RGB(v[0] * 16 + v[1], v[2] * 16 + v[3], v[4] * 16 + v[5]);
			if (len == 8) *alpha = (float)(v[6] * 16 + v[7]) / 255.0f;
		}
		return 1;
	}
	if (strcmp(low, "transparent") == 0) {
		*rgb = 0;
		*alpha = 0.0f;
		return 1;
	}
	if (strcmp(low, "currentcolor") == 0) {
		NSVGattrib* attr = nsvg__getAttr(p);
		*rgb = attr->currentColor;
		*alpha = attr->currentColorAlpha;
		return 1;
	}
	if (strncmp(low, "rgb", 3) == 0 || strncmp(low, "hsl", 3) == 0) {
		const int hsl = low[0] == 'h';
		const char* s = low + 3;
		double v[4];
		char unit[4];
		int count = 0;
		if (*s == 'a') s++;
		if (*s != '(') return 0;
		s++;
		while (count < 4) {
			if (!nsvg__cssNumber(&s, &v[count], &unit[count])) return 0;
			count++;
			while (*s && nsvg__isspace(*s)) s++;
			if (*s == ',' || *s == '/') s++;
			while (*s && nsvg__isspace(*s)) s++;
			if (*s == ')') break;
		}
		if (*s != ')' || count < 3) return 0;
		if (count == 4) {
			const double a = unit[3] == '%' ? v[3] / 100.0 : v[3];
			*alpha = (float)(a < 0 ? 0 : a > 1 ? 1 : a);
		}
		if (hsl) {
			float h = (float)fmod(v[0] / 360.0, 1.0), sat = (float)(v[1] / 100.0), l = (float)(v[2] / 100.0), m1, m2;
			if (h < 0) h += 1;
			sat = sat < 0 ? 0 : sat > 1 ? 1 : sat;
			l = l < 0 ? 0 : l > 1 ? 1 : l;
			m2 = l <= 0.5f ? l * (sat + 1) : l + sat - l * sat;
			m1 = l * 2 - m2;
			*rgb = NSVG_RGB((unsigned int)(nsvg__hue(m1, m2, h + 1.0f / 3.0f) * 255.0f + 0.5f), (unsigned int)(nsvg__hue(m1, m2, h) * 255.0f + 0.5f),
							(unsigned int)(nsvg__hue(m1, m2, h - 1.0f / 3.0f) * 255.0f + 0.5f));
		} else {
			unsigned int c[3];
			for (i = 0; i < 3; i++) {
				double x = unit[i] == '%' ? v[i] * 2.55 : v[i];
				x = x < 0 ? 0 : x > 255 ? 255 : x;
				c[i] = (unsigned int)(x + 0.5);
			}
			*rgb = NSVG_RGB(c[0], c[1], c[2]);
		}
		return 1;
	}
	for (i = 0; i < sizeof(nsvg__colors) / sizeof(NSVGNamedColor); i++)
		if (strcmp(nsvg__colors[i].name, low) == 0) {
			*rgb = nsvg__colors[i].color;
			return 1;
		}
	return 0;
}

// ImWidgets change: a number, optionally in percent.
static int nsvg__validOpacity(const char* str)
{
	double v;
	char unit;
	if (!nsvg__cssNumber(&str, &v, &unit) || unit == 'd') return 0;
	while (*str && nsvg__isspace(*str)) str++;
	return *str == 0;
}

static float nsvg__parseOpacity(const char* str)
{
	float val = nsvg__atof(str);
	if (strchr(str, '%') != NULL) val /= 100.0f;	// ImWidgets change: percentages
	if (val < 0.0f) val = 0.0f;
	if (val > 1.0f) val = 1.0f;
	return val;
}

static float nsvg__parseMiterLimit(const char* str)
{
	float val = nsvg__atof(str);
	if (val < 0.0f) val = 0.0f;
	return val;
}

static int nsvg__parseUnits(const char* units)
{
	if (units[0] == 'p' && units[1] == 'x')
		return NSVG_UNITS_PX;
	else if (units[0] == 'p' && units[1] == 't')
		return NSVG_UNITS_PT;
	else if (units[0] == 'p' && units[1] == 'c')
		return NSVG_UNITS_PC;
	else if (units[0] == 'm' && units[1] == 'm')
		return NSVG_UNITS_MM;
	else if (units[0] == 'c' && units[1] == 'm')
		return NSVG_UNITS_CM;
	else if (units[0] == 'i' && units[1] == 'n')
		return NSVG_UNITS_IN;
	else if (units[0] == '%')
		return NSVG_UNITS_PERCENT;
	else if (units[0] == 'e' && units[1] == 'm')
		return NSVG_UNITS_EM;
	else if (units[0] == 'e' && units[1] == 'x')
		return NSVG_UNITS_EX;
	return NSVG_UNITS_USER;
}

static int nsvg__isCoordinate(const char* s)
{
	// optional sign
	if (*s == '-' || *s == '+')
		s++;
	// must have at least one digit, or start by a dot
	return (nsvg__isdigit(*s) || *s == '.');
}

static NSVGcoordinate nsvg__parseCoordinateRaw(const char* str)
{
	NSVGcoordinate coord = {0, NSVG_UNITS_USER};
	char buf[64];
	coord.units = nsvg__parseUnits(nsvg__parseNumber(str, buf, 64));
	coord.value = nsvg__atof(buf);
	return coord;
}

static NSVGcoordinate nsvg__coord(float v, int units)
{
	NSVGcoordinate coord = {v, units};
	return coord;
}

static float nsvg__parseCoordinate(NSVGparser* p, const char* str, float orig, float length)
{
	NSVGcoordinate coord = nsvg__parseCoordinateRaw(str);
	return nsvg__convertToPixels(p, coord, orig, length);
}

static int nsvg__parseTransformArgs(const char* str, float* args, int maxNa, int* na)
{
	const char* end;
	const char* ptr;
	char it[64];

	*na = 0;
	ptr = str;
	while (*ptr && *ptr != '(') ++ptr;
	if (*ptr == 0)
		return 1;
	end = ptr;
	while (*end && *end != ')') ++end;
	if (*end == 0)
		return 1;

	while (ptr < end) {
		if (*ptr == '-' || *ptr == '+' || *ptr == '.' || nsvg__isdigit(*ptr)) {
			if (*na >= maxNa) return 0;
			ptr = nsvg__parseNumber(ptr, it, 64);
			args[(*na)++] = (float)nsvg__atof(it);
		} else {
			++ptr;
		}
	}
	return (int)(end - str);
}


static int nsvg__parseMatrix(float* xform, const char* str)
{
	float t[6];
	int na = 0;
	int len = nsvg__parseTransformArgs(str, t, 6, &na);
	if (na != 6) return len;
	memcpy(xform, t, sizeof(float)*6);
	return len;
}

static int nsvg__parseTranslate(float* xform, const char* str)
{
	float args[2];
	float t[6];
	int na = 0;
	int len = nsvg__parseTransformArgs(str, args, 2, &na);
	if (na == 1) args[1] = 0.0;

	nsvg__xformSetTranslation(t, args[0], args[1]);
	memcpy(xform, t, sizeof(float)*6);
	return len;
}

static int nsvg__parseScale(float* xform, const char* str)
{
	float args[2];
	int na = 0;
	float t[6];
	int len = nsvg__parseTransformArgs(str, args, 2, &na);
	if (na == 1) args[1] = args[0];
	nsvg__xformSetScale(t, args[0], args[1]);
	memcpy(xform, t, sizeof(float)*6);
	return len;
}

static int nsvg__parseSkewX(float* xform, const char* str)
{
	float args[1];
	int na = 0;
	float t[6];
	int len = nsvg__parseTransformArgs(str, args, 1, &na);
	nsvg__xformSetSkewX(t, args[0]/180.0f*NSVG_PI);
	memcpy(xform, t, sizeof(float)*6);
	return len;
}

static int nsvg__parseSkewY(float* xform, const char* str)
{
	float args[1];
	int na = 0;
	float t[6];
	int len = nsvg__parseTransformArgs(str, args, 1, &na);
	nsvg__xformSetSkewY(t, args[0]/180.0f*NSVG_PI);
	memcpy(xform, t, sizeof(float)*6);
	return len;
}

static int nsvg__parseRotate(float* xform, const char* str)
{
	float args[3];
	int na = 0;
	float m[6];
	float t[6];
	int len = nsvg__parseTransformArgs(str, args, 3, &na);
	if (na == 1)
		args[1] = args[2] = 0.0f;
	nsvg__xformIdentity(m);

	if (na > 1) {
		nsvg__xformSetTranslation(t, -args[1], -args[2]);
		nsvg__xformMultiply(m, t);
	}

	nsvg__xformSetRotation(t, args[0]/180.0f*NSVG_PI);
	nsvg__xformMultiply(m, t);

	if (na > 1) {
		nsvg__xformSetTranslation(t, args[1], args[2]);
		nsvg__xformMultiply(m, t);
	}

	memcpy(xform, m, sizeof(float)*6);

	return len;
}

static void nsvg__parseTransform(float* xform, const char* str)
{
	float t[6];
	int len;
	nsvg__xformIdentity(xform);
	while (*str)
	{
		if (strncmp(str, "matrix", 6) == 0)
			len = nsvg__parseMatrix(t, str);
		else if (strncmp(str, "translate", 9) == 0)
			len = nsvg__parseTranslate(t, str);
		else if (strncmp(str, "scale", 5) == 0)
			len = nsvg__parseScale(t, str);
		else if (strncmp(str, "rotate", 6) == 0)
			len = nsvg__parseRotate(t, str);
		else if (strncmp(str, "skewX", 5) == 0)
			len = nsvg__parseSkewX(t, str);
		else if (strncmp(str, "skewY", 5) == 0)
			len = nsvg__parseSkewY(t, str);
		else{
			++str;
			continue;
		}
		if (len != 0) {
			str += len;
		} else {
			++str;
			continue;
		}

		nsvg__xformPremultiply(xform, t);
	}
}

static void nsvg__parseUrl(char* id, const char* str)
{
	int i = 0;
	str += 4; // "url(";
	if (*str && *str == '#')
		str++;
	while (i < 63 && *str && *str != ')') {
		id[i] = *str++;
		i++;
	}
	id[i] = '\0';
}

static char nsvg__parseLineCap(const char* str)
{
	if (strcmp(str, "butt") == 0)
		return NSVG_CAP_BUTT;
	else if (strcmp(str, "round") == 0)
		return NSVG_CAP_ROUND;
	else if (strcmp(str, "square") == 0)
		return NSVG_CAP_SQUARE;
	// TODO: handle inherit.
	return NSVG_CAP_BUTT;
}

static char nsvg__parseLineJoin(const char* str)
{
	if (strcmp(str, "miter") == 0)
		return NSVG_JOIN_MITER;
	else if (strcmp(str, "round") == 0)
		return NSVG_JOIN_ROUND;
	else if (strcmp(str, "bevel") == 0)
		return NSVG_JOIN_BEVEL;
	// TODO: handle inherit.
	return NSVG_JOIN_MITER;
}

static char nsvg__parseFillRule(const char* str)
{
	if (strcmp(str, "nonzero") == 0)
		return NSVG_FILLRULE_NONZERO;
	else if (strcmp(str, "evenodd") == 0)
		return NSVG_FILLRULE_EVENODD;
	// TODO: handle inherit.
	return NSVG_FILLRULE_NONZERO;
}

static unsigned char nsvg__parsePaintOrder(const char* str)
{
	// ImWidgets change: any of the keywords in order, the others following in their default order (SVG 2).
	int order[3], used[3] = { 0, 0, 0 }, n = 0, k;
	const enum NSVGpaintOrder defaults[3] = { NSVG_PAINT_FILL, NSVG_PAINT_STROKE, NSVG_PAINT_MARKERS };
	const char* s = str;
	while (*s) {
		const char* w;
		size_t len;
		int v = -1;
		while (*s && nsvg__isspace(*s)) s++;
		if (!*s) break;
		w = s;
		while (*s && !nsvg__isspace(*s)) s++;
		len = (size_t)(s - w);
		if (len == 4 && strncmp(w, "fill", 4) == 0) v = NSVG_PAINT_FILL;
		else if (len == 6 && strncmp(w, "stroke", 6) == 0) v = NSVG_PAINT_STROKE;
		else if (len == 7 && strncmp(w, "markers", 7) == 0) v = NSVG_PAINT_MARKERS;
		if (v < 0 || used[v] || n == 3) { n = 0; break; }
		used[v] = 1;
		order[n++] = v;
	}
	if (n > 0) {
		for (k = 0; k < 3 && n < 3; k++)
			if (!used[defaults[k]])
				order[n++] = defaults[k];
		return nsvg__encodePaintOrder((enum NSVGpaintOrder)order[0], (enum NSVGpaintOrder)order[1], (enum NSVGpaintOrder)order[2]);
	}
	if (strcmp(str, "normal") == 0 || strcmp(str, "fill stroke markers") == 0)
		return nsvg__encodePaintOrder(NSVG_PAINT_FILL, NSVG_PAINT_STROKE, NSVG_PAINT_MARKERS);
	else if (strcmp(str, "fill markers stroke") == 0)
		return nsvg__encodePaintOrder(NSVG_PAINT_FILL, NSVG_PAINT_MARKERS, NSVG_PAINT_STROKE);
	else if (strcmp(str, "markers fill stroke") == 0)
		return nsvg__encodePaintOrder(NSVG_PAINT_MARKERS, NSVG_PAINT_FILL, NSVG_PAINT_STROKE);
	else if (strcmp(str, "markers stroke fill") == 0)
		return nsvg__encodePaintOrder(NSVG_PAINT_MARKERS, NSVG_PAINT_STROKE, NSVG_PAINT_FILL);
	else if (strcmp(str, "stroke fill markers") == 0)
		return nsvg__encodePaintOrder(NSVG_PAINT_STROKE, NSVG_PAINT_FILL, NSVG_PAINT_MARKERS);
	else if (strcmp(str, "stroke markers fill") == 0)
		return nsvg__encodePaintOrder(NSVG_PAINT_STROKE, NSVG_PAINT_MARKERS, NSVG_PAINT_FILL);
	// TODO: handle inherit.
	return nsvg__encodePaintOrder(NSVG_PAINT_FILL, NSVG_PAINT_STROKE, NSVG_PAINT_MARKERS);
}

static const char* nsvg__getNextDashItem(const char* s, char* it)
{
	int n = 0;
	it[0] = '\0';
	// Skip white spaces and commas
	while (*s && (nsvg__isspace(*s) || *s == ',')) s++;
	// Advance until whitespace, comma or end.
	while (*s && (!nsvg__isspace(*s) && *s != ',')) {
		if (n < 63)
			it[n++] = *s;
		s++;
	}
	it[n++] = '\0';
	return s;
}

static int nsvg__parseStrokeDashArray(NSVGparser* p, const char* str, float* strokeDashArray)
{
	char item[64];
	int count = 0, i;
	float sum = 0.0f;

	// Handle "none"
	if (str[0] == 'n')
		return 0;

	// Parse dashes
	while (*str) {
		str = nsvg__getNextDashItem(str, item);
		if (!*item) break;
		if (count < NSVG_MAX_DASHES)
			strokeDashArray[count++] = fabsf(nsvg__parseCoordinate(p, item, 0.0f, nsvg__actualLength(p)));
	}

	for (i = 0; i < count; i++)
		sum += strokeDashArray[i];
	if (sum <= 1e-6f)
		count = 0;

	return count;
}

static void nsvg__parseStyle(NSVGparser* p, const char* str);

// Apply any matching class styles for a "class" attribute value. We support only simple class
// selectors. The class attribute may contain multiple space-separated class names.
static void nsvg__applyClassStyles(NSVGparser* p, const char* value)
{
	const char* cur = value;
	while (*cur) {
		const char* classStart;
		size_t classLen;
		NSVGstyleDeclaration* style;

		while (*cur && nsvg__isspace(*cur))
			cur++;
		if (!*cur)
			break;

		classStart = cur;
		while (*cur && !nsvg__isspace(*cur))
			cur++;
		classLen = (size_t)(cur - classStart);

		for (style = p->styles; style != NULL; style = style->next) {
			if (style->className[0] == '.' && strncmp(style->className + 1, classStart, classLen) == 0 && style->className[classLen + 1] == '\0') {	// ImWidgets change: prefixed
				nsvg__parseStyle(p, style->propertiesText);
			}
		}
	}
}

static int nsvg__parseAttrStyling(NSVGparser* p, const char* name, const char* value);

// ImWidgets change: after nsvg__cascade(), styling attributes are recognized (1) without being applied again.
static int nsvg__parseAttr(NSVGparser* p, const char* name, const char* value)
{
	NSVGattrib* attr = nsvg__getAttr(p);
	NSVGattrib saved;
	int result;
	if (!p->cascaded || !attr)
		return nsvg__parseAttrStyling(p, name, value);
	saved = *attr;
	result = nsvg__parseAttrStyling(p, name, value);
	*attr = saved;
	return result;
}

// Style sheet rules whose selector is 'selector' (with its prefix), in document order (the list is head-inserted).
static void nsvg__applyRules(NSVGparser* p, const char* prefix, const char* name, size_t len)
{
	NSVGstyleDeclaration* matches[64];
	NSVGstyleDeclaration* style;
	const size_t plen = strlen(prefix);
	int n = 0;
	for (style = p->styles; style != NULL && n < 64; style = style->next)
		if (strncmp(style->className, prefix, plen) == 0 && strncmp(style->className + plen, name, len) == 0 && style->className[plen + len] == '\0')
			matches[n++] = style;
	while (n > 0)
		nsvg__parseStyle(p, matches[--n]->propertiesText);
}

// ImWidgets change: the CSS cascade of an element: presentation attributes, then style sheet rules by specificity
// (*, element, .class, #id), then the style attribute. Its own attribute loop then only reads geometry.
static void nsvg__cascade(NSVGparser* p, const char* el, const char** attr)
{
	const char* cls = NULL;
	const char* id = NULL;
	const char* style = NULL;
	int i;
	for (i = 0; attr[i]; i += 2) {
		if (strcmp(attr[i], "style") == 0) style = attr[i + 1];
		else if (strcmp(attr[i], "class") == 0) cls = attr[i + 1];
		else {
			if (strcmp(attr[i], "id") == 0) id = attr[i + 1];
			nsvg__parseAttrStyling(p, attr[i], attr[i + 1]);
		}
	}
	nsvg__applyRules(p, "", "*", 1);
	nsvg__applyRules(p, "", el, strlen(el));
	while (cls && *cls) {
		const char* start;
		while (*cls && nsvg__isspace(*cls)) cls++;
		start = cls;
		while (*cls && !nsvg__isspace(*cls)) cls++;
		if (cls > start)
			nsvg__applyRules(p, ".", start, (size_t)(cls - start));
	}
	if (id)
		nsvg__applyRules(p, "#", id, strlen(id));
	if (style)
		nsvg__parseStyle(p, style);
	p->cascaded = 1;
}

static int nsvg__parseAttrStyling(NSVGparser* p, const char* name, const char* value)
{
	float xform[6];
	NSVGattrib* attr = nsvg__getAttr(p);
	if (!attr) return 0;

	if (strcmp(name, "style") == 0) {
		nsvg__parseStyle(p, value);
	} else if (strcmp(name, "display") == 0) {
		if (strcmp(value, "none") == 0)
			attr->visible = 0;
		// Don't reset ->visible on display:inline, one display:none hides the whole subtree

	} else if (strcmp(name, "fill") == 0) {
		if (strcmp(value, "none") == 0) {
			attr->hasFill = 0;
		} else if (strncmp(value, "url(", 4) == 0) {
			attr->hasFill = 2;
			nsvg__parseUrl(attr->fillGradient, value);
		} else {
			unsigned int c;
			float a;
			if (nsvg__isCurrentColor(value)) {
				attr->hasFill = 3;		// ImWidgets change: currentColor, resolved by nsvg__addShape()
			} else if (strcmp(value, "inherit") != 0 && nsvg__parseColorCss(p, value, &c, &a)) {	// ImWidgets change
				attr->hasFill = 1;
				attr->fillColor = c;
				attr->fillColorAlpha = a;
			}
		}
	} else if (strcmp(name, "opacity") == 0) {
		if (nsvg__validOpacity(value)) attr->opacity = nsvg__parseOpacity(value);		// ImWidgets change: invalid ignored
	} else if (strcmp(name, "fill-opacity") == 0) {
		if (nsvg__validOpacity(value)) attr->fillOpacity = nsvg__parseOpacity(value);
	} else if (strcmp(name, "stroke") == 0) {
		if (strcmp(value, "none") == 0) {
			attr->hasStroke = 0;
		} else if (strncmp(value, "url(", 4) == 0) {
			attr->hasStroke = 2;
			nsvg__parseUrl(attr->strokeGradient, value);
		} else {
			unsigned int c;
			float a;
			if (nsvg__isCurrentColor(value)) {
				attr->hasStroke = 3;	// ImWidgets change: currentColor, resolved by nsvg__addShape()
			} else if (strcmp(value, "inherit") != 0 && nsvg__parseColorCss(p, value, &c, &a)) {	// ImWidgets change
				attr->hasStroke = 1;
				attr->strokeColor = c;
				attr->strokeColorAlpha = a;
			}
		}
	} else if (strcmp(name, "stroke-width") == 0) {
		attr->strokeWidth = nsvg__parseCoordinate(p, value, 0.0f, nsvg__actualLength(p));
	} else if (strcmp(name, "stroke-dasharray") == 0) {
		attr->strokeDashCount = nsvg__parseStrokeDashArray(p, value, attr->strokeDashArray);
	} else if (strcmp(name, "stroke-dashoffset") == 0) {
		attr->strokeDashOffset = nsvg__parseCoordinate(p, value, 0.0f, nsvg__actualLength(p));
	} else if (strcmp(name, "stroke-opacity") == 0) {
		if (nsvg__validOpacity(value)) attr->strokeOpacity = nsvg__parseOpacity(value);		// ImWidgets change
	} else if (strcmp(name, "stroke-linecap") == 0) {
		attr->strokeLineCap = nsvg__parseLineCap(value);
	} else if (strcmp(name, "stroke-linejoin") == 0) {
		attr->strokeLineJoin = nsvg__parseLineJoin(value);
	} else if (strcmp(name, "stroke-miterlimit") == 0) {
		attr->miterLimit = nsvg__parseMiterLimit(value);
	} else if (strcmp(name, "fill-rule") == 0) {
		attr->fillRule = nsvg__parseFillRule(value);
	} else if (strcmp(name, "font-size") == 0) {
		attr->fontSize = nsvg__parseCoordinate(p, value, 0.0f, nsvg__actualLength(p));
	} else if (strcmp(name, "mask") == 0) {		// ImWidgets change
		attr->maskId[0] = '\0';
		if (strncmp(value, "url(", 4) == 0) nsvg__parseUrl(attr->maskId, value);
	} else if (strcmp(name, "filter") == 0) {		// ImWidgets change: appended to the ancestors' filters.
		if (strncmp(value, "url(", 4) == 0 && (attr->filterOwn || attr->filterCount < NSVG_MAX_FILTERS)) {
			if (!attr->filterOwn) {
				attr->filterGroups[attr->filterCount++] = ++p->filterGroupCount;
				attr->filterOwn = 1;
			}
			nsvg__parseUrl(attr->filterIds[attr->filterCount - 1], value);
		}
	} else if (strcmp(name, "clip-path") == 0) {		// ImWidgets change: applied once the element's transform is known.
		const char* id = strstr(value, "url(#");
		attr->clipPending[0] = '\0';
		if (id != NULL) {
			int n = 0;
			id += 5;
			while (id[n] != '\0' && id[n] != ')' && n < 63) n++;
			memcpy(attr->clipPending, id, n);
			attr->clipPending[n] = '\0';
		}
	} else if (strcmp(name, "text-anchor") == 0) {		// ImWidgets change
		if (strcmp(value, "middle") == 0) attr->textAnchor = NSVG_ANCHOR_MIDDLE;
		else if (strcmp(value, "end") == 0) attr->textAnchor = NSVG_ANCHOR_END;
		else if (strcmp(value, "start") == 0) attr->textAnchor = NSVG_ANCHOR_START;
	} else if (strcmp(name, "transform") == 0) {
		nsvg__parseTransform(xform, value);
		nsvg__xformPremultiply(attr->xform, xform);
	} else if (strcmp(name, "stop-color") == 0) {
		unsigned int c;
		float a;
		if (strcmp(value, "inherit") != 0 && nsvg__parseColorCss(p, value, &c, &a)) {	// ImWidgets change
			attr->stopColor = c;
			attr->stopColorAlpha = a;
		}
	} else if (strcmp(name, "color") == 0) {
		unsigned int c;
		float a;
		if (strcmp(value, "inherit") != 0 && nsvg__parseColorCss(p, value, &c, &a)) {	// ImWidgets change: currentColor
			attr->currentColor = c;
			attr->currentColorAlpha = a;
		}
	} else if (strcmp(name, "stop-opacity") == 0) {
		if (nsvg__validOpacity(value)) attr->stopOpacity = nsvg__parseOpacity(value);		// ImWidgets change
	} else if (strcmp(name, "offset") == 0) {
		attr->stopOffset = nsvg__parseCoordinate(p, value, 0.0f, 1.0f);
	} else if (strcmp(name, "paint-order") == 0) {
		attr->paintOrder = nsvg__parsePaintOrder(value);
	} else if (strcmp(name, "id") == 0) {
		strncpy(attr->id, value, 63);
		attr->id[63] = '\0';
	} else if (strcmp(name, "class") == 0) {
		nsvg__applyClassStyles(p, value);
	}
	else {
		return 0;
	}
	return 1;
}

static int nsvg__parseNameValue(NSVGparser* p, const char* start, const char* end)
{
	const char* str;
	const char* val;
	char name[512];
	char value[512];
	int n;

	str = start;
	while (str < end && *str != ':') ++str;

	val = str;

	// Right Trim
	while (str > start &&  (*str == ':' || nsvg__isspace(*str))) --str;
	++str;

	n = (int)(str - start);
	if (n > 511) n = 511;
	if (n) memcpy(name, start, n);
	name[n] = 0;

	while (val < end && (*val == ':' || nsvg__isspace(*val))) ++val;

	n = (int)(end - val);
	if (n > 511) n = 511;
	if (n) memcpy(value, val, n);
	value[n] = 0;

	return nsvg__parseAttr(p, name, value);
}

static void nsvg__parseStyle(NSVGparser* p, const char* str)
{
	const char* start;
	const char* end;
	char* clean = NULL;

	// ImWidgets change: CSS comments removed.
	if (strstr(str, "/*") != NULL) {
		size_t n = 0;
		clean = (char*)malloc(strlen(str) + 1);
		if (clean != NULL) {
			while (*str) {
				if (str[0] == '/' && str[1] == '*') {
					const char* e = strstr(str + 2, "*/");
					str = e ? e + 2 : str + strlen(str);
				} else
					clean[n++] = *str++;
			}
			clean[n] = 0;
			str = clean;
		}
	}

	while (*str) {
		// Left Trim
		while(*str && nsvg__isspace(*str)) ++str;
		start = str;
		while(*str && *str != ';') ++str;
		end = str;

		// Right Trim
		while (end > start &&  (*end == ';' || nsvg__isspace(*end))) --end;
		++end;

		nsvg__parseNameValue(p, start, end);
		if (*str) ++str;
	}
	free(clean);
}

static void nsvg__parseAttribs(NSVGparser* p, const char** attr)
{
	int i;
	for (i = 0; attr[i]; i += 2)
	{
		if (strcmp(attr[i], "style") == 0)
			nsvg__parseStyle(p, attr[i + 1]);
		else
			nsvg__parseAttr(p, attr[i], attr[i + 1]);
	}
}

static int nsvg__getArgsPerElement(char cmd)
{
	switch (cmd) {
		case 'v':
		case 'V':
		case 'h':
		case 'H':
			return 1;
		case 'm':
		case 'M':
		case 'l':
		case 'L':
		case 't':
		case 'T':
			return 2;
		case 'q':
		case 'Q':
		case 's':
		case 'S':
			return 4;
		case 'c':
		case 'C':
			return 6;
		case 'a':
		case 'A':
			return 7;
		case 'z':
		case 'Z':
			return 0;
	}
	return -1;
}

static void nsvg__pathMoveTo(NSVGparser* p, float* cpx, float* cpy, float* args, int rel)
{
	if (rel) {
		*cpx += args[0];
		*cpy += args[1];
	} else {
		*cpx = args[0];
		*cpy = args[1];
	}
	nsvg__moveTo(p, *cpx, *cpy);
}

static void nsvg__pathLineTo(NSVGparser* p, float* cpx, float* cpy, float* args, int rel)
{
	if (rel) {
		*cpx += args[0];
		*cpy += args[1];
	} else {
		*cpx = args[0];
		*cpy = args[1];
	}
	nsvg__lineTo(p, *cpx, *cpy);
}

static void nsvg__pathHLineTo(NSVGparser* p, float* cpx, float* cpy, float* args, int rel)
{
	if (rel)
		*cpx += args[0];
	else
		*cpx = args[0];
	nsvg__lineTo(p, *cpx, *cpy);
}

static void nsvg__pathVLineTo(NSVGparser* p, float* cpx, float* cpy, float* args, int rel)
{
	if (rel)
		*cpy += args[0];
	else
		*cpy = args[0];
	nsvg__lineTo(p, *cpx, *cpy);
}

static void nsvg__pathCubicBezTo(NSVGparser* p, float* cpx, float* cpy,
								 float* cpx2, float* cpy2, float* args, int rel)
{
	float x2, y2, cx1, cy1, cx2, cy2;

	if (rel) {
		cx1 = *cpx + args[0];
		cy1 = *cpy + args[1];
		cx2 = *cpx + args[2];
		cy2 = *cpy + args[3];
		x2 = *cpx + args[4];
		y2 = *cpy + args[5];
	} else {
		cx1 = args[0];
		cy1 = args[1];
		cx2 = args[2];
		cy2 = args[3];
		x2 = args[4];
		y2 = args[5];
	}

	nsvg__cubicBezTo(p, cx1,cy1, cx2,cy2, x2,y2);

	*cpx2 = cx2;
	*cpy2 = cy2;
	*cpx = x2;
	*cpy = y2;
}

static void nsvg__pathCubicBezShortTo(NSVGparser* p, float* cpx, float* cpy,
									  float* cpx2, float* cpy2, float* args, int rel)
{
	float x1, y1, x2, y2, cx1, cy1, cx2, cy2;

	x1 = *cpx;
	y1 = *cpy;
	if (rel) {
		cx2 = *cpx + args[0];
		cy2 = *cpy + args[1];
		x2 = *cpx + args[2];
		y2 = *cpy + args[3];
	} else {
		cx2 = args[0];
		cy2 = args[1];
		x2 = args[2];
		y2 = args[3];
	}

	cx1 = 2*x1 - *cpx2;
	cy1 = 2*y1 - *cpy2;

	nsvg__cubicBezTo(p, cx1,cy1, cx2,cy2, x2,y2);

	*cpx2 = cx2;
	*cpy2 = cy2;
	*cpx = x2;
	*cpy = y2;
}

static void nsvg__pathQuadBezTo(NSVGparser* p, float* cpx, float* cpy,
								float* cpx2, float* cpy2, float* args, int rel)
{
	float x1, y1, x2, y2, cx, cy;
	float cx1, cy1, cx2, cy2;

	x1 = *cpx;
	y1 = *cpy;
	if (rel) {
		cx = *cpx + args[0];
		cy = *cpy + args[1];
		x2 = *cpx + args[2];
		y2 = *cpy + args[3];
	} else {
		cx = args[0];
		cy = args[1];
		x2 = args[2];
		y2 = args[3];
	}

	// Convert to cubic bezier
	cx1 = x1 + 2.0f/3.0f*(cx - x1);
	cy1 = y1 + 2.0f/3.0f*(cy - y1);
	cx2 = x2 + 2.0f/3.0f*(cx - x2);
	cy2 = y2 + 2.0f/3.0f*(cy - y2);

	nsvg__cubicBezTo(p, cx1,cy1, cx2,cy2, x2,y2);

	*cpx2 = cx;
	*cpy2 = cy;
	*cpx = x2;
	*cpy = y2;
}

static void nsvg__pathQuadBezShortTo(NSVGparser* p, float* cpx, float* cpy,
									 float* cpx2, float* cpy2, float* args, int rel)
{
	float x1, y1, x2, y2, cx, cy;
	float cx1, cy1, cx2, cy2;

	x1 = *cpx;
	y1 = *cpy;
	if (rel) {
		x2 = *cpx + args[0];
		y2 = *cpy + args[1];
	} else {
		x2 = args[0];
		y2 = args[1];
	}

	cx = 2*x1 - *cpx2;
	cy = 2*y1 - *cpy2;

	// Convert to cubix bezier
	cx1 = x1 + 2.0f/3.0f*(cx - x1);
	cy1 = y1 + 2.0f/3.0f*(cy - y1);
	cx2 = x2 + 2.0f/3.0f*(cx - x2);
	cy2 = y2 + 2.0f/3.0f*(cy - y2);

	nsvg__cubicBezTo(p, cx1,cy1, cx2,cy2, x2,y2);

	*cpx2 = cx;
	*cpy2 = cy;
	*cpx = x2;
	*cpy = y2;
}

static float nsvg__sqr(float x) { return x*x; }
static float nsvg__vmag(float x, float y) { return sqrtf(x*x + y*y); }

static float nsvg__vecrat(float ux, float uy, float vx, float vy)
{
	return (ux*vx + uy*vy) / (nsvg__vmag(ux,uy) * nsvg__vmag(vx,vy));
}

static float nsvg__vecang(float ux, float uy, float vx, float vy)
{
	float r = nsvg__vecrat(ux,uy, vx,vy);
	if (r < -1.0f) r = -1.0f;
	if (r > 1.0f) r = 1.0f;
	return ((ux*vy < uy*vx) ? -1.0f : 1.0f) * acosf(r);
}

static void nsvg__pathArcTo(NSVGparser* p, float* cpx, float* cpy, float* args, int rel)
{
	// Ported from canvg (https://code.google.com/p/canvg/)
	float rx, ry, rotx;
	float x1, y1, x2, y2, cx, cy, dx, dy, d;
	float x1p, y1p, cxp, cyp, s, sa, sb;
	float ux, uy, vx, vy, a1, da;
	float x, y, tanx, tany, a, px = 0, py = 0, ptanx = 0, ptany = 0, t[6];
	float sinrx, cosrx;
	int fa, fs;
	int i, ndivs;
	float hda, kappa;

	rx = fabsf(args[0]);				// y radius
	ry = fabsf(args[1]);				// x radius
	rotx = args[2] / 180.0f * NSVG_PI;		// x rotation angle
	fa = fabsf(args[3]) > 1e-6 ? 1 : 0;	// Large arc
	fs = fabsf(args[4]) > 1e-6 ? 1 : 0;	// Sweep direction
	x1 = *cpx;							// start point
	y1 = *cpy;
	if (rel) {							// end point
		x2 = *cpx + args[5];
		y2 = *cpy + args[6];
	} else {
		x2 = args[5];
		y2 = args[6];
	}

	dx = x1 - x2;
	dy = y1 - y2;
	d = sqrtf(dx*dx + dy*dy);
	if (d < 1e-6f || rx < 1e-6f || ry < 1e-6f) {
		// The arc degenerates to a line
		nsvg__lineTo(p, x2, y2);
		*cpx = x2;
		*cpy = y2;
		return;
	}

	sinrx = sinf(rotx);
	cosrx = cosf(rotx);

	// Convert to center point parameterization.
	// http://www.w3.org/TR/SVG11/implnote.html#ArcImplementationNotes
	// 1) Compute x1', y1'
	x1p = cosrx * dx / 2.0f + sinrx * dy / 2.0f;
	y1p = -sinrx * dx / 2.0f + cosrx * dy / 2.0f;
	d = nsvg__sqr(x1p)/nsvg__sqr(rx) + nsvg__sqr(y1p)/nsvg__sqr(ry);
	if (d > 1) {
		d = sqrtf(d);
		rx *= d;
		ry *= d;
	}
	// 2) Compute cx', cy'
	s = 0.0f;
	sa = nsvg__sqr(rx)*nsvg__sqr(ry) - nsvg__sqr(rx)*nsvg__sqr(y1p) - nsvg__sqr(ry)*nsvg__sqr(x1p);
	sb = nsvg__sqr(rx)*nsvg__sqr(y1p) + nsvg__sqr(ry)*nsvg__sqr(x1p);
	if (sa < 0.0f) sa = 0.0f;
	if (sb > 0.0f)
		s = sqrtf(sa / sb);
	if (fa == fs)
		s = -s;
	cxp = s * rx * y1p / ry;
	cyp = s * -ry * x1p / rx;

	// 3) Compute cx,cy from cx',cy'
	cx = (x1 + x2)/2.0f + cosrx*cxp - sinrx*cyp;
	cy = (y1 + y2)/2.0f + sinrx*cxp + cosrx*cyp;

	// 4) Calculate theta1, and delta theta.
	ux = (x1p - cxp) / rx;
	uy = (y1p - cyp) / ry;
	vx = (-x1p - cxp) / rx;
	vy = (-y1p - cyp) / ry;
	a1 = nsvg__vecang(1.0f,0.0f, ux,uy);	// Initial angle
	da = nsvg__vecang(ux,uy, vx,vy);		// Delta angle

//	if (vecrat(ux,uy,vx,vy) <= -1.0f) da = NSVG_PI;
//	if (vecrat(ux,uy,vx,vy) >= 1.0f) da = 0;

	if (fs == 0 && da > 0)
		da -= 2 * NSVG_PI;
	else if (fs == 1 && da < 0)
		da += 2 * NSVG_PI;

	// Approximate the arc using cubic spline segments.
	t[0] = cosrx; t[1] = sinrx;
	t[2] = -sinrx; t[3] = cosrx;
	t[4] = cx; t[5] = cy;

	// Split arc into max 90 degree segments.
	// The loop assumes an iteration per end point (including start and end), this +1.
	ndivs = (int)(fabsf(da) / (NSVG_PI*0.5f) + 1.0f);
	hda = (da / (float)ndivs) / 2.0f;
	// Fix for ticket #179: division by 0: avoid cotangens around 0 (infinite)
	if ((hda < 1e-3f) && (hda > -1e-3f))
		hda *= 0.5f;
	else
		hda = (1.0f - cosf(hda)) / sinf(hda);
	kappa = fabsf(4.0f / 3.0f * hda);
	if (da < 0.0f)
		kappa = -kappa;

	for (i = 0; i <= ndivs; i++) {
		a = a1 + da * ((float)i/(float)ndivs);
		dx = cosf(a);
		dy = sinf(a);
		nsvg__xformPoint(&x, &y, dx*rx, dy*ry, t); // position
		nsvg__xformVec(&tanx, &tany, -dy*rx * kappa, dx*ry * kappa, t); // tangent
		if (i > 0)
			nsvg__cubicBezTo(p, px+ptanx,py+ptany, x-tanx, y-tany, x, y);
		px = x;
		py = y;
		ptanx = tanx;
		ptany = tany;
	}

	*cpx = x2;
	*cpy = y2;
}

static void nsvg__parsePath(NSVGparser* p, const char** attr)
{
	const char* s = NULL;
	char cmd = '\0';
	float args[10];
	int nargs;
	int rargs = 0;
	char initPoint;
	float cpx, cpy, cpx2, cpy2;
	const char* tmp[4];
	char closedFlag;
	int i;
	char item[64];

	for (i = 0; attr[i]; i += 2) {
		if (strcmp(attr[i], "d") == 0) {
			s = attr[i + 1];
		} else {
			tmp[0] = attr[i];
			tmp[1] = attr[i + 1];
			tmp[2] = 0;
			tmp[3] = 0;
			nsvg__parseAttribs(p, tmp);
		}
	}

	if (s) {
		nsvg__resetPath(p);
		cpx = 0; cpy = 0;
		cpx2 = 0; cpy2 = 0;
		initPoint = 0;
		closedFlag = 0;
		nargs = 0;

		while (*s) {
			item[0] = '\0';
			if ((cmd == 'A' || cmd == 'a') && (nargs == 3 || nargs == 4))
				s = nsvg__getNextPathItemWhenArcFlag(s, item);
			if (!*item)
				s = nsvg__getNextPathItem(s, item);
			if (!*item) break;
			if (cmd != '\0' && nsvg__isCoordinate(item)) {
				if (nargs < 10)
					args[nargs++] = (float)nsvg__atof(item);
				if (nargs >= rargs) {
					switch (cmd) {
						case 'm':
						case 'M':
							nsvg__pathMoveTo(p, &cpx, &cpy, args, cmd == 'm' ? 1 : 0);
							// Moveto can be followed by multiple coordinate pairs,
							// which should be treated as linetos.
							cmd = (cmd == 'm') ? 'l' : 'L';
							rargs = nsvg__getArgsPerElement(cmd);
							cpx2 = cpx; cpy2 = cpy;
							initPoint = 1;
							break;
						case 'l':
						case 'L':
							nsvg__pathLineTo(p, &cpx, &cpy, args, cmd == 'l' ? 1 : 0);
							cpx2 = cpx; cpy2 = cpy;
							break;
						case 'H':
						case 'h':
							nsvg__pathHLineTo(p, &cpx, &cpy, args, cmd == 'h' ? 1 : 0);
							cpx2 = cpx; cpy2 = cpy;
							break;
						case 'V':
						case 'v':
							nsvg__pathVLineTo(p, &cpx, &cpy, args, cmd == 'v' ? 1 : 0);
							cpx2 = cpx; cpy2 = cpy;
							break;
						case 'C':
						case 'c':
							nsvg__pathCubicBezTo(p, &cpx, &cpy, &cpx2, &cpy2, args, cmd == 'c' ? 1 : 0);
							break;
						case 'S':
						case 's':
							nsvg__pathCubicBezShortTo(p, &cpx, &cpy, &cpx2, &cpy2, args, cmd == 's' ? 1 : 0);
							break;
						case 'Q':
						case 'q':
							nsvg__pathQuadBezTo(p, &cpx, &cpy, &cpx2, &cpy2, args, cmd == 'q' ? 1 : 0);
							break;
						case 'T':
						case 't':
							nsvg__pathQuadBezShortTo(p, &cpx, &cpy, &cpx2, &cpy2, args, cmd == 't' ? 1 : 0);
							break;
						case 'A':
						case 'a':
							nsvg__pathArcTo(p, &cpx, &cpy, args, cmd == 'a' ? 1 : 0);
							cpx2 = cpx; cpy2 = cpy;
							break;
						default:
							if (nargs >= 2) {
								cpx = args[nargs-2];
								cpy = args[nargs-1];
								cpx2 = cpx; cpy2 = cpy;
							}
							break;
					}
					nargs = 0;
				}
			} else {
				cmd = item[0];
				if (cmd == 'M' || cmd == 'm') {
					// Commit path.
					if (p->npts > 0)
						nsvg__addPath(p, closedFlag);
					// Start new subpath.
					nsvg__resetPath(p);
					closedFlag = 0;
					nargs = 0;
				} else if (initPoint == 0) {
					// Do not allow other commands until initial point has been set (moveTo called once).
					cmd = '\0';
				}
				if (cmd == 'Z' || cmd == 'z') {
					closedFlag = 1;
					// Commit path.
					if (p->npts > 0) {
						// Move current point to first point
						cpx = p->pts[0];
						cpy = p->pts[1];
						cpx2 = cpx; cpy2 = cpy;
						nsvg__addPath(p, closedFlag);
					}
					// Start new subpath.
					nsvg__resetPath(p);
					nsvg__moveTo(p, cpx, cpy);
					closedFlag = 0;
					nargs = 0;
				}
				rargs = nsvg__getArgsPerElement(cmd);
				if (rargs == -1) {
					// Command not recognized
					cmd = '\0';
					rargs = 0;
				}
			}
		}
		// Commit path.
		if (p->npts)
			nsvg__addPath(p, closedFlag);
	}

	nsvg__addShape(p);
}

static void nsvg__parseRect(NSVGparser* p, const char** attr)
{
	float x = 0.0f;
	float y = 0.0f;
	float w = 0.0f;
	float h = 0.0f;
	float rx = -1.0f; // marks not set
	float ry = -1.0f;
	int i;

	for (i = 0; attr[i]; i += 2) {
		if (!nsvg__parseAttr(p, attr[i], attr[i + 1])) {
			if (strcmp(attr[i], "x") == 0) x = nsvg__parseCoordinate(p, attr[i+1], nsvg__actualOrigX(p), nsvg__actualWidth(p));
			if (strcmp(attr[i], "y") == 0) y = nsvg__parseCoordinate(p, attr[i+1], nsvg__actualOrigY(p), nsvg__actualHeight(p));
			if (strcmp(attr[i], "width") == 0) w = nsvg__parseCoordinate(p, attr[i+1], 0.0f, nsvg__actualWidth(p));
			if (strcmp(attr[i], "height") == 0) h = nsvg__parseCoordinate(p, attr[i+1], 0.0f, nsvg__actualHeight(p));
			if (strcmp(attr[i], "rx") == 0) rx = fabsf(nsvg__parseCoordinate(p, attr[i+1], 0.0f, nsvg__actualWidth(p)));
			if (strcmp(attr[i], "ry") == 0) ry = fabsf(nsvg__parseCoordinate(p, attr[i+1], 0.0f, nsvg__actualHeight(p)));
		}
	}

	if (rx < 0.0f && ry > 0.0f) rx = ry;
	if (ry < 0.0f && rx > 0.0f) ry = rx;
	if (rx < 0.0f) rx = 0.0f;
	if (ry < 0.0f) ry = 0.0f;
	if (rx > w/2.0f) rx = w/2.0f;
	if (ry > h/2.0f) ry = h/2.0f;

	if (w != 0.0f && h != 0.0f) {
		nsvg__resetPath(p);

		if (rx < 0.00001f || ry < 0.0001f) {
			nsvg__moveTo(p, x, y);
			nsvg__lineTo(p, x+w, y);
			nsvg__lineTo(p, x+w, y+h);
			nsvg__lineTo(p, x, y+h);
		} else {
			// Rounded rectangle
			nsvg__moveTo(p, x+rx, y);
			nsvg__lineTo(p, x+w-rx, y);
			nsvg__cubicBezTo(p, x+w-rx*(1-NSVG_KAPPA90), y, x+w, y+ry*(1-NSVG_KAPPA90), x+w, y+ry);
			nsvg__lineTo(p, x+w, y+h-ry);
			nsvg__cubicBezTo(p, x+w, y+h-ry*(1-NSVG_KAPPA90), x+w-rx*(1-NSVG_KAPPA90), y+h, x+w-rx, y+h);
			nsvg__lineTo(p, x+rx, y+h);
			nsvg__cubicBezTo(p, x+rx*(1-NSVG_KAPPA90), y+h, x, y+h-ry*(1-NSVG_KAPPA90), x, y+h-ry);
			nsvg__lineTo(p, x, y+ry);
			nsvg__cubicBezTo(p, x, y+ry*(1-NSVG_KAPPA90), x+rx*(1-NSVG_KAPPA90), y, x+rx, y);
		}

		nsvg__addPath(p, 1);

		nsvg__addShape(p);
	}
}

static void nsvg__parseCircle(NSVGparser* p, const char** attr)
{
	float cx = 0.0f;
	float cy = 0.0f;
	float r = 0.0f;
	int i;

	for (i = 0; attr[i]; i += 2) {
		if (!nsvg__parseAttr(p, attr[i], attr[i + 1])) {
			if (strcmp(attr[i], "cx") == 0) cx = nsvg__parseCoordinate(p, attr[i+1], nsvg__actualOrigX(p), nsvg__actualWidth(p));
			if (strcmp(attr[i], "cy") == 0) cy = nsvg__parseCoordinate(p, attr[i+1], nsvg__actualOrigY(p), nsvg__actualHeight(p));
			if (strcmp(attr[i], "r") == 0) r = fabsf(nsvg__parseCoordinate(p, attr[i+1], 0.0f, nsvg__actualLength(p)));
		}
	}

	if (r > 0.0f) {
		nsvg__resetPath(p);

		nsvg__moveTo(p, cx+r, cy);
		nsvg__cubicBezTo(p, cx+r, cy+r*NSVG_KAPPA90, cx+r*NSVG_KAPPA90, cy+r, cx, cy+r);
		nsvg__cubicBezTo(p, cx-r*NSVG_KAPPA90, cy+r, cx-r, cy+r*NSVG_KAPPA90, cx-r, cy);
		nsvg__cubicBezTo(p, cx-r, cy-r*NSVG_KAPPA90, cx-r*NSVG_KAPPA90, cy-r, cx, cy-r);
		nsvg__cubicBezTo(p, cx+r*NSVG_KAPPA90, cy-r, cx+r, cy-r*NSVG_KAPPA90, cx+r, cy);

		nsvg__addPath(p, 1);

		nsvg__addShape(p);
	}
}

static void nsvg__parseEllipse(NSVGparser* p, const char** attr)
{
	float cx = 0.0f;
	float cy = 0.0f;
	float rx = 0.0f;
	float ry = 0.0f;
	int i;

	for (i = 0; attr[i]; i += 2) {
		if (!nsvg__parseAttr(p, attr[i], attr[i + 1])) {
			if (strcmp(attr[i], "cx") == 0) cx = nsvg__parseCoordinate(p, attr[i+1], nsvg__actualOrigX(p), nsvg__actualWidth(p));
			if (strcmp(attr[i], "cy") == 0) cy = nsvg__parseCoordinate(p, attr[i+1], nsvg__actualOrigY(p), nsvg__actualHeight(p));
			if (strcmp(attr[i], "rx") == 0) rx = fabsf(nsvg__parseCoordinate(p, attr[i+1], 0.0f, nsvg__actualWidth(p)));
			if (strcmp(attr[i], "ry") == 0) ry = fabsf(nsvg__parseCoordinate(p, attr[i+1], 0.0f, nsvg__actualHeight(p)));
		}
	}

	if (rx > 0.0f && ry > 0.0f) {

		nsvg__resetPath(p);

		nsvg__moveTo(p, cx+rx, cy);
		nsvg__cubicBezTo(p, cx+rx, cy+ry*NSVG_KAPPA90, cx+rx*NSVG_KAPPA90, cy+ry, cx, cy+ry);
		nsvg__cubicBezTo(p, cx-rx*NSVG_KAPPA90, cy+ry, cx-rx, cy+ry*NSVG_KAPPA90, cx-rx, cy);
		nsvg__cubicBezTo(p, cx-rx, cy-ry*NSVG_KAPPA90, cx-rx*NSVG_KAPPA90, cy-ry, cx, cy-ry);
		nsvg__cubicBezTo(p, cx+rx*NSVG_KAPPA90, cy-ry, cx+rx, cy-ry*NSVG_KAPPA90, cx+rx, cy);

		nsvg__addPath(p, 1);

		nsvg__addShape(p);
	}
}

static void nsvg__parseLine(NSVGparser* p, const char** attr)
{
	float x1 = 0.0;
	float y1 = 0.0;
	float x2 = 0.0;
	float y2 = 0.0;
	int i;

	for (i = 0; attr[i]; i += 2) {
		if (!nsvg__parseAttr(p, attr[i], attr[i + 1])) {
			if (strcmp(attr[i], "x1") == 0) x1 = nsvg__parseCoordinate(p, attr[i + 1], nsvg__actualOrigX(p), nsvg__actualWidth(p));
			if (strcmp(attr[i], "y1") == 0) y1 = nsvg__parseCoordinate(p, attr[i + 1], nsvg__actualOrigY(p), nsvg__actualHeight(p));
			if (strcmp(attr[i], "x2") == 0) x2 = nsvg__parseCoordinate(p, attr[i + 1], nsvg__actualOrigX(p), nsvg__actualWidth(p));
			if (strcmp(attr[i], "y2") == 0) y2 = nsvg__parseCoordinate(p, attr[i + 1], nsvg__actualOrigY(p), nsvg__actualHeight(p));
		}
	}

	nsvg__resetPath(p);

	nsvg__moveTo(p, x1, y1);
	nsvg__lineTo(p, x2, y2);

	nsvg__addPath(p, 0);

	nsvg__addShape(p);
}

static void nsvg__parsePoly(NSVGparser* p, const char** attr, int closeFlag)
{
	int i;
	const char* s;
	float args[2];
	int nargs, npts = 0;
	char item[64];

	nsvg__resetPath(p);

	for (i = 0; attr[i]; i += 2) {
		if (!nsvg__parseAttr(p, attr[i], attr[i + 1])) {
			if (strcmp(attr[i], "points") == 0) {
				s = attr[i + 1];
				nargs = 0;
				while (*s) {
					s = nsvg__getNextPathItem(s, item);
					args[nargs++] = (float)nsvg__atof(item);
					if (nargs >= 2) {
						if (npts == 0)
							nsvg__moveTo(p, args[0], args[1]);
						else
							nsvg__lineTo(p, args[0], args[1]);
						nargs = 0;
						npts++;
					}
				}
			}
		}
	}

	nsvg__addPath(p, (char)closeFlag);

	nsvg__addShape(p);
}

static void nsvg__parseSVG(NSVGparser* p, const char** attr)
{
	int i;
	for (i = 0; attr[i]; i += 2) {
		if (!nsvg__parseAttr(p, attr[i], attr[i + 1])) {
			if (strcmp(attr[i], "width") == 0) {
				p->image->width = nsvg__parseCoordinate(p, attr[i + 1], 0.0f, 0.0f);
			} else if (strcmp(attr[i], "height") == 0) {
				p->image->height = nsvg__parseCoordinate(p, attr[i + 1], 0.0f, 0.0f);
			} else if (strcmp(attr[i], "viewBox") == 0) {
				const char *s = attr[i + 1];
				char buf[64];
				s = nsvg__parseNumber(s, buf, 64);
				p->viewMinx = nsvg__atof(buf);
				while (*s && (nsvg__isspace(*s) || *s == '%' || *s == ',')) s++;
				if (!*s) return;
				s = nsvg__parseNumber(s, buf, 64);
				p->viewMiny = nsvg__atof(buf);
				while (*s && (nsvg__isspace(*s) || *s == '%' || *s == ',')) s++;
				if (!*s) return;
				s = nsvg__parseNumber(s, buf, 64);
				p->viewWidth = nsvg__atof(buf);
				while (*s && (nsvg__isspace(*s) || *s == '%' || *s == ',')) s++;
				if (!*s) return;
				s = nsvg__parseNumber(s, buf, 64);
				p->viewHeight = nsvg__atof(buf);
			} else if (strcmp(attr[i], "preserveAspectRatio") == 0) {
				if (strstr(attr[i + 1], "none") != 0) {
					// No uniform scaling
					p->alignType = NSVG_ALIGN_NONE;
				} else {
					// Parse X align
					if (strstr(attr[i + 1], "xMin") != 0)
						p->alignX = NSVG_ALIGN_MIN;
					else if (strstr(attr[i + 1], "xMid") != 0)
						p->alignX = NSVG_ALIGN_MID;
					else if (strstr(attr[i + 1], "xMax") != 0)
						p->alignX = NSVG_ALIGN_MAX;
					// Parse X align
					if (strstr(attr[i + 1], "yMin") != 0)
						p->alignY = NSVG_ALIGN_MIN;
					else if (strstr(attr[i + 1], "yMid") != 0)
						p->alignY = NSVG_ALIGN_MID;
					else if (strstr(attr[i + 1], "yMax") != 0)
						p->alignY = NSVG_ALIGN_MAX;
					// Parse meet/slice
					p->alignType = NSVG_ALIGN_MEET;
					if (strstr(attr[i + 1], "slice") != 0)
						p->alignType = NSVG_ALIGN_SLICE;
				}
			}
		}
	}
}

static void nsvg__parseGradient(NSVGparser* p, const char** attr, signed char type)
{
	int i;
	int plainHref = 0;		// ImWidgets change
	NSVGgradientData* grad = (NSVGgradientData*)malloc(sizeof(NSVGgradientData));
	if (grad == NULL) return;
	memset(grad, 0, sizeof(NSVGgradientData));
	grad->units = NSVG_OBJECT_SPACE;
	grad->type = type;
	if (grad->type == NSVG_PAINT_LINEAR_GRADIENT) {
		grad->linear.x1 = nsvg__coord(0.0f, NSVG_UNITS_PERCENT);
		grad->linear.y1 = nsvg__coord(0.0f, NSVG_UNITS_PERCENT);
		grad->linear.x2 = nsvg__coord(100.0f, NSVG_UNITS_PERCENT);
		grad->linear.y2 = nsvg__coord(0.0f, NSVG_UNITS_PERCENT);
	} else if (grad->type == NSVG_PAINT_RADIAL_GRADIENT) {
		grad->radial.fr = nsvg__coord(0.0f, NSVG_UNITS_USER);		// ImWidgets change
		grad->radial.cx = nsvg__coord(50.0f, NSVG_UNITS_PERCENT);
		grad->radial.cy = nsvg__coord(50.0f, NSVG_UNITS_PERCENT);
		grad->radial.r = nsvg__coord(50.0f, NSVG_UNITS_PERCENT);
	}

	nsvg__xformIdentity(grad->xform);

	for (i = 0; attr[i]; i += 2) {
		if (strcmp(attr[i], "id") == 0) {
			strncpy(grad->id, attr[i+1], 63);
			grad->id[63] = '\0';
		} else if (!nsvg__parseAttr(p, attr[i], attr[i + 1])) {
			if (strcmp(attr[i], "gradientUnits") == 0) {
				// ImWidgets change: invalid values keep the default (objectBoundingBox).
				if (strcmp(attr[i+1], "userSpaceOnUse") == 0) {
					grad->units = NSVG_USER_SPACE;
					grad->set |= NSVG_SET_UNITS;
				} else if (strcmp(attr[i+1], "objectBoundingBox") == 0) {
					grad->units = NSVG_OBJECT_SPACE;
					grad->set |= NSVG_SET_UNITS;
				}
			} else if (strcmp(attr[i], "gradientTransform") == 0) {
				nsvg__parseTransform(grad->xform, attr[i + 1]);
				grad->set |= NSVG_SET_XFORM;
			} else if (strcmp(attr[i], "cx") == 0) {
				grad->radial.cx = nsvg__parseCoordinateRaw(attr[i + 1]);
				grad->set |= NSVG_SET_CX;
			} else if (strcmp(attr[i], "cy") == 0) {
				grad->radial.cy = nsvg__parseCoordinateRaw(attr[i + 1]);
				grad->set |= NSVG_SET_CY;
			} else if (strcmp(attr[i], "r") == 0) {
				grad->radial.r = nsvg__parseCoordinateRaw(attr[i + 1]);
				grad->set |= NSVG_SET_R;
			} else if (strcmp(attr[i], "fx") == 0) {
				grad->radial.fx = nsvg__parseCoordinateRaw(attr[i + 1]);
				grad->set |= NSVG_SET_FX;
			} else if (strcmp(attr[i], "fy") == 0) {
				grad->radial.fy = nsvg__parseCoordinateRaw(attr[i + 1]);
				grad->set |= NSVG_SET_FY;
			} else if (strcmp(attr[i], "fr") == 0) {
				grad->radial.fr = nsvg__parseCoordinateRaw(attr[i + 1]);
				grad->set |= NSVG_SET_FR;
			} else if (strcmp(attr[i], "x1") == 0) {
				grad->linear.x1 = nsvg__parseCoordinateRaw(attr[i + 1]);
				grad->set |= NSVG_SET_X1;
			} else if (strcmp(attr[i], "y1") == 0) {
				grad->linear.y1 = nsvg__parseCoordinateRaw(attr[i + 1]);
				grad->set |= NSVG_SET_Y1;
			} else if (strcmp(attr[i], "x2") == 0) {
				grad->linear.x2 = nsvg__parseCoordinateRaw(attr[i + 1]);
				grad->set |= NSVG_SET_X2;
			} else if (strcmp(attr[i], "y2") == 0) {
				grad->linear.y2 = nsvg__parseCoordinateRaw(attr[i + 1]);
				grad->set |= NSVG_SET_Y2;
			} else if (strcmp(attr[i], "spreadMethod") == 0) {
				if (strcmp(attr[i+1], "pad") == 0)
					grad->spread = NSVG_SPREAD_PAD;
				else if (strcmp(attr[i+1], "reflect") == 0)
					grad->spread = NSVG_SPREAD_REFLECT;
				else if (strcmp(attr[i+1], "repeat") == 0)
					grad->spread = NSVG_SPREAD_REPEAT;
				if (strcmp(attr[i+1], "pad") == 0 || strcmp(attr[i+1], "reflect") == 0 || strcmp(attr[i+1], "repeat") == 0)
					grad->set |= NSVG_SET_SPREAD;		// ImWidgets change
			} else if (strcmp(attr[i], "href") == 0 || (strcmp(attr[i], "xlink:href") == 0 && !plainHref)) {	// ImWidgets change: href (SVG 2) first
				const char *href = attr[i+1];
				plainHref |= strcmp(attr[i], "href") == 0;
				strncpy(grad->ref, href+1, 62);
				grad->ref[62] = '\0';
			}
		}
	}

	grad->next = p->gradients;
	p->gradients = grad;
}

static void nsvg__parseGradientStop(NSVGparser* p, const char** attr)
{
	NSVGattrib* curAttr = nsvg__getAttr(p);
	NSVGgradientData* grad;
	NSVGgradientStop* stop;
	int i, idx;

	curAttr->stopOffset = 0;
	curAttr->stopColor = 0;
	curAttr->stopOpacity = 1.0f;
	curAttr->stopColorAlpha = 1.0f;		// ImWidgets change

	for (i = 0; attr[i]; i += 2) {
		nsvg__parseAttr(p, attr[i], attr[i + 1]);
	}

	// Add stop to the last gradient.
	grad = p->gradients;
	if (grad == NULL) return;

	grad->nstops++;
	grad->stops = (NSVGgradientStop*)realloc(grad->stops, sizeof(NSVGgradientStop)*grad->nstops);
	if (grad->stops == NULL) return;

	// ImWidgets change: appended, the offset clamped to [0, 1] and to the previous stop's (SVG 1.1, 13.2.4); was sorted.
	idx = grad->nstops-1;
	if (curAttr->stopOffset < 0.0f) curAttr->stopOffset = 0.0f;
	if (curAttr->stopOffset > 1.0f) curAttr->stopOffset = 1.0f;
	if (idx > 0 && curAttr->stopOffset < grad->stops[idx-1].offset)
		curAttr->stopOffset = grad->stops[idx-1].offset;

	stop = &grad->stops[idx];
	stop->color = curAttr->stopColor;
	stop->color |= (unsigned int)(curAttr->stopOpacity*curAttr->stopColorAlpha*255) << 24;
	stop->offset = curAttr->stopOffset;
}

// ImWidgets change: <text> and <tspan>. Each run of text with one style and position becomes a shape (see NSVGshape::text).
static void nsvg__textBytes(NSVGparser* p, const char* b, int n)
{
	if (p->textLen + n + 1 > p->textCap) {
		int cap = p->textCap ? p->textCap * 2 : 64;
		char* buf;
		while (cap < p->textLen + n + 1) cap *= 2;
		buf = (char*)realloc(p->textBuf, cap);
		if (buf == NULL) return;
		p->textBuf = buf;
		p->textCap = cap;
	}
	memcpy(p->textBuf + p->textLen, b, n);
	p->textLen += n;
	p->textBuf[p->textLen] = '\0';
}

static void nsvg__textChar(NSVGparser* p, unsigned int c)
{
	char utf8[4];
	int n = 0;
	if (c < 0x80) utf8[n++] = (char)c;
	else if (c < 0x800) { utf8[n++] = (char)(0xC0 | (c >> 6)); utf8[n++] = (char)(0x80 | (c & 0x3F)); }
	else if (c < 0x10000) { utf8[n++] = (char)(0xE0 | (c >> 12)); utf8[n++] = (char)(0x80 | ((c >> 6) & 0x3F)); utf8[n++] = (char)(0x80 | (c & 0x3F)); }
	else if (c < 0x110000) { utf8[n++] = (char)(0xF0 | (c >> 18)); utf8[n++] = (char)(0x80 | ((c >> 12) & 0x3F)); utf8[n++] = (char)(0x80 | ((c >> 6) & 0x3F)); utf8[n++] = (char)(0x80 | (c & 0x3F)); }
	nsvg__textBytes(p, utf8, n);
}

// Character data with entities decoded and white space collapsed (xml:space default).
static void nsvg__appendText(NSVGparser* p, const char* s)
{
	while (*s) {
		unsigned int c = (unsigned char)*s++;
		if (c == '&') {
			const char* e = strchr(s, ';');
			if (e != NULL && e - s <= 10) {
				if (*s == '#') c = (unsigned int)(s[1] == 'x' || s[1] == 'X' ? strtoul(s + 2, NULL, 16) : strtoul(s + 1, NULL, 10));
				else if (strncmp(s, "amp;", 4) == 0) c = '&';
				else if (strncmp(s, "lt;", 3) == 0) c = '<';
				else if (strncmp(s, "gt;", 3) == 0) c = '>';
				else if (strncmp(s, "quot;", 5) == 0) c = '"';
				else if (strncmp(s, "apos;", 5) == 0) c = '\'';
				s = e + 1;
			}
		} else if (c >= 0x80) {		// UTF-8 bytes as they are.
			const char b = (char)c;
			nsvg__textBytes(p, &b, 1);
			p->textSpace = 0;
			continue;
		}
		if (nsvg__isspace((char)c)) {
			if (p->textSpace) continue;
			c = ' ';
			p->textSpace = 1;
		} else {
			p->textSpace = 0;
		}
		nsvg__textChar(p, c);
	}
}

// The run read so far as a shape at the text position; the next run continues after it.
static void nsvg__flushText(NSVGparser* p, int trim_end)
{
	NSVGattrib* attr = nsvg__getAttr(p);
	NSVGshape* tail = p->shapesTail;
	if (trim_end)
		while (p->textLen > 0 && p->textBuf[p->textLen - 1] == ' ') p->textBuf[--p->textLen] = '\0';
	if (p->textLen == 0)
		return;
	nsvg__resetPath(p);
	nsvg__addPoint(p, p->textX, p->textY);
	nsvg__addPoint(p, p->textX + 1.0f, p->textY);
	nsvg__addPoint(p, p->textX, p->textY + 1.0f);
	nsvg__addPoint(p, p->textX, p->textY);
	nsvg__addPath(p, 0);
	nsvg__addShape(p);
	if (p->shapesTail != tail && p->shapesTail != NULL) {
		NSVGshape* shape = p->shapesTail;
		shape->text = (char*)malloc(p->textLen + 1);
		if (shape->text != NULL) memcpy(shape->text, p->textBuf, p->textLen + 1);
		shape->fontSize = attr->fontSize;
		shape->textAnchor = attr->textAnchor;
		shape->textContinues = p->textContinues;
	}
	p->textLen = 0;
	p->textContinues = 1;
}

// x, y, dx, dy of <text> or <tspan> (the first value of lists); x starts a new chunk.
static void nsvg__parseTextPosition(NSVGparser* p, const char** attr)
{
	int i;
	for (i = 0; attr[i]; i += 2) {
		if (!nsvg__parseAttr(p, attr[i], attr[i + 1])) {
			if (strcmp(attr[i], "x") == 0) {
				p->textX = nsvg__parseCoordinate(p, attr[i+1], nsvg__actualOrigX(p), nsvg__actualWidth(p));
				p->textContinues = 0;
			} else if (strcmp(attr[i], "y") == 0) {
				p->textY = nsvg__parseCoordinate(p, attr[i+1], nsvg__actualOrigY(p), nsvg__actualHeight(p));
			} else if (strcmp(attr[i], "dy") == 0) {
				p->textY += nsvg__parseCoordinate(p, attr[i+1], 0.0f, nsvg__actualHeight(p));
			} else if (strcmp(attr[i], "dx") == 0) {
				p->textX += nsvg__parseCoordinate(p, attr[i+1], 0.0f, nsvg__actualWidth(p));
			}
		}
	}
}

// ImWidgets change: <image>, a shape outlined by what shows of it: its innermost clip path through the transform that
// clip was referenced with (PDF conversions cut images to curves), else its rectangle.
static void nsvg__parseImage(NSVGparser* p, const char** attr)
{
	NSVGattrib* a;
	const char* href = NULL;
	float x = 0.0f, y = 0.0f, w = 0.0f, h = 0.0f, saved[6], c[6];
	int i;
	NSVGshape* tail = p->shapesTail;
	for (i = 0; attr[i]; i += 2) {
		if (!nsvg__parseAttr(p, attr[i], attr[i + 1])) {
			if (strcmp(attr[i], "x") == 0) x = nsvg__parseCoordinate(p, attr[i+1], nsvg__actualOrigX(p), nsvg__actualWidth(p));
			if (strcmp(attr[i], "y") == 0) y = nsvg__parseCoordinate(p, attr[i+1], nsvg__actualOrigY(p), nsvg__actualHeight(p));
			if (strcmp(attr[i], "width") == 0) w = nsvg__parseCoordinate(p, attr[i+1], 0.0f, nsvg__actualWidth(p));
			if (strcmp(attr[i], "height") == 0) h = nsvg__parseCoordinate(p, attr[i+1], 0.0f, nsvg__actualHeight(p));
			if (strcmp(attr[i], "href") == 0 || strcmp(attr[i], "xlink:href") == 0) href = attr[i + 1];
		}
	}
	if (w <= 0.0f || h <= 0.0f || href == NULL)
		return;
	nsvg__applyClip(p);
	a = nsvg__getAttr(p);
	nsvg__xformPoint(&c[0], &c[1], x, y, a->xform);
	nsvg__xformPoint(&c[2], &c[3], x + w, y, a->xform);
	nsvg__xformPoint(&c[4], &c[5], x, y + h, a->xform);
	if (a->clipData != NULL && a->clipData->paths != NULL) {
		NSVGpath* cp;
		memcpy(saved, a->xform, sizeof(saved));
		memcpy(a->xform, a->clipXform, sizeof(saved));
		for (cp = a->clipData->paths; cp != NULL; cp = cp->next) {
			nsvg__resetPath(p);
			for (i = 0; i < cp->npts; i++)
				nsvg__addPoint(p, cp->pts[i * 2], cp->pts[i * 2 + 1]);
			nsvg__addPath(p, 1);
		}
		memcpy(a->xform, saved, sizeof(saved));
	} else {
		nsvg__resetPath(p);
		nsvg__moveTo(p, x, y);
		nsvg__lineTo(p, x + w, y);
		nsvg__lineTo(p, x + w, y + h);
		nsvg__lineTo(p, x, y + h);
		nsvg__addPath(p, 1);
	}
	nsvg__addShape(p);
	if (p->shapesTail != tail && p->shapesTail != NULL) {
		NSVGshape* shape = p->shapesTail;
		const size_t n = strlen(href);
		shape->image = (char*)malloc(n + 1);
		if (shape->image != NULL) memcpy(shape->image, href, n + 1);
		memcpy(shape->imageCorners, c, sizeof(c));
	}
}

// ImWidgets change: a filter primitive's attributes, appended to the filter being read.
static void nsvg__parseFilterPrim(NSVGparser* p, const char* el, const char** attr)
{
	NSVGfilterPrim* prim = (NSVGfilterPrim*)calloc(1, sizeof(NSVGfilterPrim));
	NSVGfilterPrim** tail;
	float floodOpacity = 1.0f;
	int i;
	if (prim == NULL) return;
	if (strcmp(el, "feFlood") == 0) prim->type = NSVG_FE_FLOOD;
	else if (strcmp(el, "feBlend") == 0) prim->type = NSVG_FE_BLEND;
	else if (strcmp(el, "feColorMatrix") == 0) prim->type = NSVG_FE_COLOR_MATRIX;
	else if (strcmp(el, "feMorphology") == 0) prim->type = NSVG_FE_MORPHOLOGY;
	else if (strcmp(el, "feOffset") == 0) prim->type = NSVG_FE_OFFSET;
	else if (strcmp(el, "feGaussianBlur") == 0) prim->type = NSVG_FE_GAUSSIAN_BLUR;
	else if (strcmp(el, "feComposite") == 0) prim->type = NSVG_FE_COMPOSITE;
	for (i = 0; attr[i]; i += 2) {
		const char* name = attr[i];
		const char* value = attr[i + 1];
		char it[64];
		if (strcmp(name, "in") == 0) strncpy(prim->in, value, 31);
		else if (strcmp(name, "in2") == 0) strncpy(prim->in2, value, 31);
		else if (strcmp(name, "result") == 0) strncpy(prim->result, value, 31);
		else if (strcmp(name, "operator") == 0 || strcmp(name, "mode") == 0 || strcmp(name, "type") == 0) strncpy(prim->op, value, 15);
		else if (strcmp(name, "dx") == 0) prim->dx = (float)nsvg__atof(value);
		else if (strcmp(name, "dy") == 0) prim->dy = (float)nsvg__atof(value);
		else if (strcmp(name, "stdDeviation") == 0) prim->stdDeviation = (float)nsvg__atof(value);
		else if (strcmp(name, "radius") == 0) prim->radius = (float)nsvg__atof(value);
		else if (name[0] == 'k' && name[1] >= '1' && name[1] <= '4' && name[2] == '\0') prim->k[name[1] - '1'] = (float)nsvg__atof(value);
		else if (strcmp(name, "flood-color") == 0) prim->floodColor = nsvg__parseColor(value);
		else if (strcmp(name, "flood-opacity") == 0) floodOpacity = (float)nsvg__atof(value);
		else if (strcmp(name, "values") == 0) {
			const char* v = value;
			while (prim->nvalues < 20) {
				v = nsvg__getNextPathItem(v, it);
				if (it[0] == '\0') break;
				prim->values[prim->nvalues++] = (float)nsvg__atof(it);
			}
		}
	}
	floodOpacity = floodOpacity < 0.0f ? 0.0f : floodOpacity > 1.0f ? 1.0f : floodOpacity;
	prim->floodColor = (prim->floodColor & 0xffffff) | ((unsigned int)(floodOpacity * 255.0f) << 24);
	for (tail = &p->filterCurrent->prims; *tail != NULL; tail = &(*tail)->next) {}
	*tail = prim;
}

static void nsvg__startElement(void* ud, const char* el, const char** attr)
{
	NSVGparser* p = (NSVGparser*)ud;

	if (strcmp(el, "filter") == 0) {		// ImWidgets change: its primitives, for the caller.
		NSVGfilter* f = (NSVGfilter*)calloc(1, sizeof(NSVGfilter));
		int i;
		if (f == NULL) return;
		for (i = 0; attr[i]; i += 2)
			if (strcmp(attr[i], "id") == 0) strncpy(f->id, attr[i + 1], 63);
		f->next = p->image->filters;
		p->image->filters = f;
		p->filterCurrent = f;
		return;
	}
	if (p->filterCurrent != NULL) {
		if (el[0] == 'f' && el[1] == 'e')
			nsvg__parseFilterPrim(p, el, attr);
		return;
	}
	if (strcmp(el, "pattern") == 0) {		// ImWidgets change: its attributes; its shapes are made in its space, tagged with its id.
		NSVGpattern* pat = (NSVGpattern*)calloc(1, sizeof(NSVGpattern));
		NSVGattrib* a;
		int i, plainHref = 0;
		if (pat == NULL) return;
		nsvg__xformIdentity(pat->xform);
		for (i = 0; attr[i]; i += 2) {
			const char* name = attr[i];
			const char* value = attr[i + 1];
			if (strcmp(name, "id") == 0) strncpy(pat->id, value, 63);
			else if (strcmp(name, "href") == 0 || (strcmp(name, "xlink:href") == 0 && !plainHref)) {
				plainHref |= strcmp(name, "href") == 0;
				strncpy(pat->href, value[0] == '#' ? value + 1 : value, 63);
			}
			else if (strcmp(name, "x") == 0) { pat->x = (float)nsvg__atof(value); pat->set |= NSVG_PATTERN_SET_X; }
			else if (strcmp(name, "y") == 0) { pat->y = (float)nsvg__atof(value); pat->set |= NSVG_PATTERN_SET_Y; }
			else if (strcmp(name, "width") == 0) { pat->width = (float)nsvg__atof(value); pat->set |= NSVG_PATTERN_SET_WIDTH; }
			else if (strcmp(name, "height") == 0) { pat->height = (float)nsvg__atof(value); pat->set |= NSVG_PATTERN_SET_HEIGHT; }
			else if (strcmp(name, "patternUnits") == 0) { pat->userSpace = strcmp(value, "userSpaceOnUse") == 0; pat->set |= NSVG_PATTERN_SET_UNITS; }
			else if (strcmp(name, "patternContentUnits") == 0) { pat->contentBox = strcmp(value, "objectBoundingBox") == 0; pat->set |= NSVG_PATTERN_SET_CONTENT_UNITS; }
			else if (strcmp(name, "patternTransform") == 0) { nsvg__parseTransform(pat->xform, value); pat->set |= NSVG_PATTERN_SET_XFORM; }
		}
		pat->next = p->image->patterns;
		p->image->patterns = pat;
		nsvg__pushAttr(p);
		a = nsvg__getAttr(p);
		nsvg__xformIdentity(a->xform);
		a->hasClip = 0;
		a->clipData = NULL;
		a->maskId[0] = '\0';
		a->filterCount = 0;
		strncpy(p->patternCurrent, pat->id, 63);
		return;
	}
	if (strcmp(el, "mask") == 0) {		// ImWidgets change: its shapes are made, tagged with its id.
		int i;
		nsvg__pushAttr(p);
		nsvg__getAttr(p)->hasClip = 0;
		nsvg__getAttr(p)->clipData = NULL;
		nsvg__getAttr(p)->maskId[0] = '\0';
		nsvg__getAttr(p)->filterCount = 0;
		nsvg__cascade(p, el, attr);
		p->cascaded = 0;
		for (i = 0; attr[i]; i += 2)
			if (strcmp(attr[i], "id") == 0) strncpy(p->maskCurrent, attr[i + 1], 63);
		return;
	}

	if (strcmp(el, "clipPath") == 0) {		// ImWidgets change: its bounds, from the shapes inside.
		NSVGclipData* c = (NSVGclipData*)calloc(1, sizeof(NSVGclipData));
		int i;
		if (c == NULL) return;
		c->empty = 1;
		for (i = 0; attr[i]; i += 2)
			if (strcmp(attr[i], "id") == 0) strncpy(c->id, attr[i + 1], 63);
		c->next = p->clips;
		p->clips = c;
		p->clipCurrent = c;
		nsvg__pushAttr(p);
		nsvg__getAttr(p)->hasClip = 0;
		return;
	}
	if (p->defsFlag && p->clipCurrent == NULL && p->maskCurrent[0] == '\0' && p->patternCurrent[0] == '\0') {
		// Skip everything but gradients and styles in defs
		if (strcmp(el, "linearGradient") == 0) {
			nsvg__parseGradient(p, attr, NSVG_PAINT_LINEAR_GRADIENT);
		} else if (strcmp(el, "radialGradient") == 0) {
			nsvg__parseGradient(p, attr, NSVG_PAINT_RADIAL_GRADIENT);
		} else if (strcmp(el, "stop") == 0) {
			nsvg__parseGradientStop(p, attr);
		} else if (strcmp(el, "style") == 0) {
			p->styleFlag = 1;
		}
		return;
	}

	if (strcmp(el, "g") == 0) {
		nsvg__pushAttr(p);
		nsvg__cascade(p, el, attr);		// ImWidgets change
		nsvg__applyClip(p);				// ImWidgets change
		p->cascaded = 0;
	} else if (strcmp(el, "text") == 0) {		// ImWidgets change
		nsvg__pushAttr(p);
		nsvg__cascade(p, el, attr);
		p->textX = p->textY = 0.0f;
		p->textLen = 0;
		nsvg__parseTextPosition(p, attr);
		p->cascaded = 0;
		p->textContinues = 0;
		p->textSpace = 1;		// Leading spaces are dropped.
		p->textFlag = 1;
	} else if (strcmp(el, "tspan") == 0 && p->textFlag) {		// ImWidgets change
		nsvg__flushText(p, 0);
		nsvg__pushAttr(p);
		nsvg__cascade(p, el, attr);
		nsvg__parseTextPosition(p, attr);
		p->cascaded = 0;
	} else if (strcmp(el, "image") == 0) {		// ImWidgets change
		nsvg__pushAttr(p);
		nsvg__cascade(p, el, attr);
		nsvg__parseImage(p, attr);
		p->cascaded = 0;
		nsvg__popAttr(p);
	} else if (strcmp(el, "path") == 0) {
		if (p->pathFlag)	// Do not allow nested paths.
			return;
		nsvg__pushAttr(p);
		nsvg__cascade(p, el, attr);		// ImWidgets change
		nsvg__parsePath(p, attr);
		p->cascaded = 0;
		nsvg__popAttr(p);
	} else if (strcmp(el, "rect") == 0) {
		nsvg__pushAttr(p);
		nsvg__cascade(p, el, attr);		// ImWidgets change
		nsvg__parseRect(p, attr);
		p->cascaded = 0;
		nsvg__popAttr(p);
	} else if (strcmp(el, "circle") == 0) {
		nsvg__pushAttr(p);
		nsvg__cascade(p, el, attr);		// ImWidgets change
		nsvg__parseCircle(p, attr);
		p->cascaded = 0;
		nsvg__popAttr(p);
	} else if (strcmp(el, "ellipse") == 0) {
		nsvg__pushAttr(p);
		nsvg__cascade(p, el, attr);		// ImWidgets change
		nsvg__parseEllipse(p, attr);
		p->cascaded = 0;
		nsvg__popAttr(p);
	} else if (strcmp(el, "line") == 0)  {
		nsvg__pushAttr(p);
		nsvg__cascade(p, el, attr);		// ImWidgets change
		nsvg__parseLine(p, attr);
		p->cascaded = 0;
		nsvg__popAttr(p);
	} else if (strcmp(el, "polyline") == 0)  {
		nsvg__pushAttr(p);
		nsvg__cascade(p, el, attr);		// ImWidgets change
		nsvg__parsePoly(p, attr, 0);
		p->cascaded = 0;
		nsvg__popAttr(p);
	} else if (strcmp(el, "polygon") == 0)  {
		nsvg__pushAttr(p);
		nsvg__cascade(p, el, attr);		// ImWidgets change
		nsvg__parsePoly(p, attr, 1);
		p->cascaded = 0;
		nsvg__popAttr(p);
	} else  if (strcmp(el, "linearGradient") == 0) {
		nsvg__parseGradient(p, attr, NSVG_PAINT_LINEAR_GRADIENT);
	} else if (strcmp(el, "radialGradient") == 0) {
		nsvg__parseGradient(p, attr, NSVG_PAINT_RADIAL_GRADIENT);
	} else if (strcmp(el, "stop") == 0) {
		nsvg__parseGradientStop(p, attr);
	} else if (strcmp(el, "defs") == 0) {
		p->defsFlag = 1;
	} else if (strcmp(el, "svg") == 0) {
		nsvg__parseSVG(p, attr);
	} else if (strcmp(el, "style") == 0) {
		p->styleFlag = 1;
	}
}

static void nsvg__endElement(void* ud, const char* el)
{
	NSVGparser* p = (NSVGparser*)ud;

	if (strcmp(el, "g") == 0) {
		nsvg__popAttr(p);
	} else if (strcmp(el, "clipPath") == 0 && p->clipCurrent != NULL) {		// ImWidgets change
		p->clipCurrent = NULL;
		nsvg__popAttr(p);
	} else if (strcmp(el, "filter") == 0) {		// ImWidgets change
		p->filterCurrent = NULL;
	} else if (strcmp(el, "mask") == 0 && p->maskCurrent[0] != '\0') {		// ImWidgets change
		p->maskCurrent[0] = '\0';
		nsvg__popAttr(p);
	} else if (strcmp(el, "pattern") == 0 && p->patternCurrent[0] != '\0') {		// ImWidgets change
		p->patternCurrent[0] = '\0';
		nsvg__popAttr(p);
	} else if (strcmp(el, "text") == 0 && p->textFlag) {		// ImWidgets change
		nsvg__flushText(p, 1);
		p->textFlag = 0;
		nsvg__popAttr(p);
	} else if (strcmp(el, "tspan") == 0 && p->textFlag) {		// ImWidgets change
		nsvg__flushText(p, 0);
		nsvg__popAttr(p);
	} else if (strcmp(el, "path") == 0) {
		p->pathFlag = 0;
	} else if (strcmp(el, "defs") == 0) {
		p->defsFlag = 0;
	} else if (strcmp(el, "style") == 0) {
		p->styleFlag = 0;
	}
}

static char *nsvg__strndup(const char *s, size_t n)
{
	char *result = (char *)malloc(n + 1);
	if (result == NULL)
		return NULL;

	memcpy(result, s, n);
	result[n] = '\0';
	return result;
}

static void nsvg__content(void* ud, const char* s)
{
	NSVGparser* p = (NSVGparser*)ud;
	if (p->textFlag) {		// ImWidgets change
		nsvg__appendText(p, s);
		return;
	}
	if (!p->styleFlag)
		return;
	while (*s && nsvg__isspace(*s)) s++;

	// Parse all the styles inside the style block. Each style's content will be later processed using nsvg__parseStyle().
	// Note: We only support selector lists of simple class selectors (e.g. ".foo, .bar { ... }").
	while (*s) {
		NSVGstyleDeclaration* styles[NSVG_MAX_CLASSES];
		int nstyles = 0;
		const char* propsStart;
		const char* propsEnd;
		int i;

		// 1) Parse the selector list up to '{'. For each simple class selector ('.name'),
		//    allocate a new NSVGstyleDeclaration into the local staging array. Styles are
		//    only committed to p->styles in step 3 below, once their propertiesText has
		//    also been allocated successfully.
		while (*s && *s != '{') {
			const char* selStart;
			const char* selEnd;
			NSVGstyleDeclaration* style;

			while (*s && (nsvg__isspace(*s) || *s == ','))
				s++;
			if (!*s || *s == '{')
				break;

			// ImWidgets change: one comma separated selector; simple ones only (*, element, .class, #id), kept with
			// their prefix. Combinators, attribute selectors, pseudo-classes and compounds are skipped.
			selStart = s;
			while (*s && *s != ',' && *s != '{')
				s++;
			selEnd = s;
			while (selEnd > selStart && nsvg__isspace(selEnd[-1]))
				selEnd--;
			{
				const char* c;
				int simple = selEnd > selStart;
				for (c = selStart; c < selEnd && simple; c++)
					if (nsvg__isspace(*c) || *c == '>' || *c == '+' || *c == '~' || *c == '[' || *c == ':' || ((*c == '.' || *c == '#') && c != selStart))
						simple = 0;
				if (!simple || nstyles >= NSVG_MAX_CLASSES)
					continue;
			}

			style = (NSVGstyleDeclaration*)malloc(sizeof(NSVGstyleDeclaration));
			if (style == NULL)
				continue;
			style->className = nsvg__strndup(selStart, (size_t)(selEnd - selStart));
			if (style->className == NULL) {
				free(style);
				continue;
			}
			style->propertiesText = NULL;
			style->next = NULL;
			styles[nstyles++] = style;
		}
		if (!*s) {
			// No '{' found - discard pending styles and stop.
			for (i = 0; i < nstyles; i++) {
				free(styles[i]->className);
				free(styles[i]);
			}
			break;
		}
		s++; // advance past '{'

		// 2) Find the end of the properties block (up to '}').
		propsStart = s;
		while (*s && *s != '}')
			s++;
		propsEnd = s;

		// 3) Allocate propertiesText for each pending style. Commit successful ones to
		//    p->styles (head-insertion); free any whose allocation fails.
		for (i = 0; i < nstyles; i++) {
			styles[i]->propertiesText = nsvg__strndup(propsStart, (size_t)(propsEnd - propsStart));
			if (styles[i]->propertiesText == NULL) {
				free(styles[i]->className);
				free(styles[i]);
			} else {
				styles[i]->next = p->styles;
				p->styles = styles[i];
			}
		}

		if (*s)
			s++; // advance past '}'
	}
}

static void nsvg__imageBounds(NSVGparser* p, float* bounds)
{
	NSVGshape* shape;
	shape = p->image->shapes;
	if (shape == NULL) {
		bounds[0] = bounds[1] = bounds[2] = bounds[3] = 0.0;
		return;
	}
	bounds[0] = shape->bounds[0];
	bounds[1] = shape->bounds[1];
	bounds[2] = shape->bounds[2];
	bounds[3] = shape->bounds[3];
	for (shape = shape->next; shape != NULL; shape = shape->next) {
		bounds[0] = nsvg__minf(bounds[0], shape->bounds[0]);
		bounds[1] = nsvg__minf(bounds[1], shape->bounds[1]);
		bounds[2] = nsvg__maxf(bounds[2], shape->bounds[2]);
		bounds[3] = nsvg__maxf(bounds[3], shape->bounds[3]);
	}
}

static float nsvg__viewAlign(float content, float container, int type)
{
	if (type == NSVG_ALIGN_MIN)
		return 0;
	else if (type == NSVG_ALIGN_MAX)
		return container - content;
	// mid
	return (container - content) * 0.5f;
}

static void nsvg__scaleGradient(NSVGgradient* grad, float tx, float ty, float sx, float sy)
{
	float t[6];
	nsvg__xformSetTranslation(t, tx, ty);
	nsvg__xformMultiply (grad->xform, t);

	nsvg__xformSetScale(t, sx, sy);
	nsvg__xformMultiply (grad->xform, t);
}

static void nsvg__scaleToViewbox(NSVGparser* p, const char* units)
{
	NSVGshape* shape;
	NSVGpath* path;
	float tx, ty, sx, sy, us, bounds[4], t[6], avgs;
	int i;
	float* pt;

	// Guess image size if not set completely.
	nsvg__imageBounds(p, bounds);

	if (p->viewWidth == 0) {
		if (p->image->width > 0) {
			p->viewWidth = p->image->width;
		} else {
			p->viewMinx = bounds[0];
			p->viewWidth = bounds[2] - bounds[0];
		}
	}
	if (p->viewHeight == 0) {
		if (p->image->height > 0) {
			p->viewHeight = p->image->height;
		} else {
			p->viewMiny = bounds[1];
			p->viewHeight = bounds[3] - bounds[1];
		}
	}
	if (p->image->width == 0)
		p->image->width = p->viewWidth;
	if (p->image->height == 0)
		p->image->height = p->viewHeight;

	tx = -p->viewMinx;
	ty = -p->viewMiny;
	sx = p->viewWidth > 0 ? p->image->width / p->viewWidth : 0;
	sy = p->viewHeight > 0 ? p->image->height / p->viewHeight : 0;
	// Unit scaling
	us = 1.0f / nsvg__convertToPixels(p, nsvg__coord(1.0f, nsvg__parseUnits(units)), 0.0f, 1.0f);

	// Fix aspect ratio
	if (p->alignType == NSVG_ALIGN_MEET) {
		// fit whole image into viewbox
		sx = sy = nsvg__minf(sx, sy);
		tx += nsvg__viewAlign(p->viewWidth*sx, p->image->width, p->alignX) / sx;
		ty += nsvg__viewAlign(p->viewHeight*sy, p->image->height, p->alignY) / sy;
	} else if (p->alignType == NSVG_ALIGN_SLICE) {
		// fill whole viewbox with image
		sx = sy = nsvg__maxf(sx, sy);
		tx += nsvg__viewAlign(p->viewWidth*sx, p->image->width, p->alignX) / sx;
		ty += nsvg__viewAlign(p->viewHeight*sy, p->image->height, p->alignY) / sy;
	}

	// Transform
	sx *= us;
	sy *= us;
	avgs = (sx+sy) / 2.0f;
	for (shape = p->image->shapes; shape != NULL; shape = shape->next) {
		for (i = 0; i < 3; i++) {		// ImWidgets change: image corners
			shape->imageCorners[i * 2] = (shape->imageCorners[i * 2] + tx) * sx;
			shape->imageCorners[i * 2 + 1] = (shape->imageCorners[i * 2 + 1] + ty) * sy;
		}
		shape->clipBounds[0] = (shape->clipBounds[0] + tx) * sx;		// ImWidgets change
		shape->clipBounds[1] = (shape->clipBounds[1] + ty) * sy;
		shape->clipBounds[2] = (shape->clipBounds[2] + tx) * sx;
		shape->clipBounds[3] = (shape->clipBounds[3] + ty) * sy;
		shape->bounds[0] = (shape->bounds[0] + tx) * sx;
		shape->bounds[1] = (shape->bounds[1] + ty) * sy;
		shape->bounds[2] = (shape->bounds[2] + tx) * sx;
		shape->bounds[3] = (shape->bounds[3] + ty) * sy;
		for (path = shape->paths; path != NULL; path = path->next) {
			path->bounds[0] = (path->bounds[0] + tx) * sx;
			path->bounds[1] = (path->bounds[1] + ty) * sy;
			path->bounds[2] = (path->bounds[2] + tx) * sx;
			path->bounds[3] = (path->bounds[3] + ty) * sy;
			for (i =0; i < path->npts; i++) {
				pt = &path->pts[i*2];
				pt[0] = (pt[0] + tx) * sx;
				pt[1] = (pt[1] + ty) * sy;
			}
		}

		if (shape->fill.type == NSVG_PAINT_LINEAR_GRADIENT || shape->fill.type == NSVG_PAINT_RADIAL_GRADIENT) {
			nsvg__scaleGradient(shape->fill.gradient, tx,ty, sx,sy);
			memcpy(t, shape->fill.gradient->xform, sizeof(float)*6);
			nsvg__xformInverse(shape->fill.gradient->xform, t);
		}
		if (shape->stroke.type == NSVG_PAINT_LINEAR_GRADIENT || shape->stroke.type == NSVG_PAINT_RADIAL_GRADIENT) {
			nsvg__scaleGradient(shape->stroke.gradient, tx,ty, sx,sy);
			memcpy(t, shape->stroke.gradient->xform, sizeof(float)*6);
			nsvg__xformInverse(shape->stroke.gradient->xform, t);
		}

		shape->strokeWidth *= avgs;
		shape->strokeDashOffset *= avgs;
		for (i = 0; i < shape->strokeDashCount; i++)
			shape->strokeDashArray[i] *= avgs;
	}
}

static void nsvg__createGradients(NSVGparser* p)
{
	NSVGshape* shape;

	for (shape = p->image->shapes; shape != NULL; shape = shape->next) {
		if (shape->fill.type == NSVG_PAINT_UNDEF) {
			if (shape->fillGradient[0] != '\0') {
				float inv[6], localBounds[4];
				nsvg__xformInverse(inv, shape->xform);
				nsvg__getLocalBounds(localBounds, shape, inv);
				shape->fill.gradient = nsvg__createGradient(p, shape->fillGradient, localBounds, shape->xform, &shape->fill.type);
			}
			if (shape->fill.type == NSVG_PAINT_UNDEF) {
				shape->fill.type = NSVG_PAINT_NONE;
			}
		}
		if (shape->stroke.type == NSVG_PAINT_UNDEF) {
			if (shape->strokeGradient[0] != '\0') {
				float inv[6], localBounds[4];
				nsvg__xformInverse(inv, shape->xform);
				nsvg__getLocalBounds(localBounds, shape, inv);
				shape->stroke.gradient = nsvg__createGradient(p, shape->strokeGradient, localBounds, shape->xform, &shape->stroke.type);
			}
			if (shape->stroke.type == NSVG_PAINT_UNDEF) {
				shape->stroke.type = NSVG_PAINT_NONE;
			}
		}
	}
}

NSVGimage* nsvgParse(char* input, const char* units, float dpi)
{
	NSVGparser* p;
	NSVGimage* ret = 0;

	p = nsvg__createParser();
	if (p == NULL) {
		return NULL;
	}
	p->dpi = dpi;

	nsvg__parseXML(input, nsvg__startElement, nsvg__endElement, nsvg__content, p);

	// Create gradients after all definitions have been parsed
	nsvg__createGradients(p);

	// Scale to viewBox
	nsvg__scaleToViewbox(p, units);

	ret = p->image;
	p->image = NULL;

	nsvg__deleteParser(p);

	return ret;
}

NSVGimage* nsvgParseFromFile(const char* filename, const char* units, float dpi)
{
	FILE* fp = NULL;
	size_t size;
	char* data = NULL;
	NSVGimage* image = NULL;

	fp = fopen(filename, "rb");
	if (!fp) goto error;
	if (fseek(fp, 0, SEEK_END)) goto error;
	size = ftell(fp);
	if (size == -1l) goto error;
	if (fseek(fp, 0, SEEK_SET)) goto error;
	data = (char*)malloc(size+1);
	if (data == NULL) goto error;
	if (fread(data, 1, size, fp) != size) goto error;
	data[size] = '\0';	// Must be null terminated.
	fclose(fp);
	image = nsvgParse(data, units, dpi);
	free(data);

	return image;

error:
	if (fp) fclose(fp);
	if (data) free(data);
	if (image) nsvgDelete(image);
	return NULL;
}

NSVGpath* nsvgDuplicatePath(NSVGpath* p)
{
    NSVGpath* res = NULL;

    if (p == NULL)
        return NULL;

    res = (NSVGpath*)malloc(sizeof(NSVGpath));
    if (res == NULL) goto error;
    memset(res, 0, sizeof(NSVGpath));

    res->pts = (float*)malloc(p->npts*2*sizeof(float));
    if (res->pts == NULL) goto error;
    memcpy(res->pts, p->pts, p->npts * sizeof(float) * 2);
    res->npts = p->npts;

    memcpy(res->bounds, p->bounds, sizeof(p->bounds));

    res->closed = p->closed;

    return res;

error:
    if (res != NULL) {
        free(res->pts);
        free(res);
    }
    return NULL;
}

void nsvgDelete(NSVGimage* image)
{
	NSVGshape *snext, *shape;
	if (image == NULL) return;
	shape = image->shapes;
	while (shape != NULL) {
		snext = shape->next;
		nsvg__deletePaths(shape->paths);
		nsvg__deletePaint(&shape->fill);
		nsvg__deletePaint(&shape->stroke);
		free(shape->text);		// ImWidgets change
		free(shape->image);
		free(shape);
		shape = snext;
	}
	while (image->patterns != NULL) {		// ImWidgets change
		NSVGpattern* pat = image->patterns;
		image->patterns = pat->next;
		free(pat);
	}
	while (image->filters != NULL) {		// ImWidgets change
		NSVGfilter* f = image->filters;
		image->filters = f->next;
		while (f->prims != NULL) {
			NSVGfilterPrim* prim = f->prims;
			f->prims = prim->next;
			free(prim);
		}
		free(f);
	}
	free(image);
}

#endif // NANOSVG_IMPLEMENTATION

#endif // NANOSVG_H
