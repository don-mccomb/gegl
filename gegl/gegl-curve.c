/* This file is part of GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Copyright 2006 Mark Probst <mark.probst@gmail.com>
 */

#include <glib.h>
#include <glib/gprintf.h>
#include <glib-object.h>
#include <string.h>
#include <stdlib.h>

#include "gegl-types.h"
#include "gegl-curve.h"

enum
{
  PROP_0,
};

typedef struct _GeglCurvePoint   GeglCurvePoint;
typedef struct _GeglCurvePrivate GeglCurvePrivate;
typedef struct _CurveNameEntity  CurveNameEntity;

struct _GeglCurvePoint
{
  gfloat x;
  gfloat y;
  gfloat y2;
};

struct _GeglCurvePrivate
{
  gfloat y_min;
  gfloat y_max;
  GArray *points;
  gboolean need_recalc;
  GeglCurvePoint **indir;
};

static void      finalize     (GObject      *self);
static void      set_property (GObject      *gobject,
                               guint         prop_id,
                               const GValue *value,
                               GParamSpec   *pspec);
static void      get_property (GObject    *gobject,
                               guint       prop_id,
                               GValue     *value,
                               GParamSpec *pspec);

G_DEFINE_TYPE (GeglCurve, gegl_curve, G_TYPE_OBJECT);

#define GEGL_CURVE_GET_PRIVATE(o)    (G_TYPE_INSTANCE_GET_PRIVATE ((o), GEGL_TYPE_CURVE, GeglCurvePrivate))

static void
gegl_curve_init (GeglCurve *self)
{
  GeglCurvePrivate *priv = GEGL_CURVE_GET_PRIVATE (self);

  priv->y_min = 0.0;
  priv->y_max = 1.0;
  priv->need_recalc = FALSE;
  priv->indir = NULL;
  priv->points = g_array_new(FALSE, FALSE, sizeof(GeglCurvePoint));
}

static void
gegl_curve_class_init (GeglCurveClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize     = finalize;
  gobject_class->set_property = set_property;
  gobject_class->get_property = get_property;

  g_type_class_add_private (klass, sizeof (GeglCurvePrivate));
}

static void
finalize (GObject *gobject)
{
  GeglCurve *self = GEGL_CURVE (gobject);
  GeglCurvePrivate *priv = GEGL_CURVE_GET_PRIVATE (self);

  g_array_free(priv->points, TRUE);
  priv->points = NULL;

  if (priv->indir != NULL)
  {
    g_free(priv->indir);
    priv->indir = NULL;
  }

  G_OBJECT_CLASS (gegl_curve_parent_class)->finalize (gobject);
}

static void
set_property (GObject      *gobject,
              guint         property_id,
              const GValue *value,
              GParamSpec   *pspec)
{
  switch (property_id)
    {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, property_id, pspec);
        break;
    }
}

static void
get_property (GObject    *gobject,
              guint       property_id,
              GValue     *value,
              GParamSpec *pspec)
{
  switch (property_id)
    {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, property_id, pspec);
        break;
    }
}

GeglCurve *
gegl_curve_new (gfloat y_min,
               gfloat y_max)
{
  GeglCurve *self = GEGL_CURVE (g_object_new (GEGL_TYPE_CURVE, NULL));
  GeglCurvePrivate *priv = GEGL_CURVE_GET_PRIVATE (self);

  gegl_curve_init(self);
  priv->y_min = y_min;
  priv->y_max = y_max;

  return self;
}

guint
gegl_curve_add_point (GeglCurve *self,
                      gfloat     x,
                      gfloat     y)
{
  GeglCurvePrivate *priv = GEGL_CURVE_GET_PRIVATE (GEGL_CURVE (self));
  GeglCurvePoint point = { x, y };

  g_array_append_val(priv->points, point);

  priv->need_recalc = TRUE;

  return priv->points->len - 1;
}

void
gegl_curve_get_point (GeglCurve      *self,
                     guint          index,
                     gfloat         *x,
                     gfloat         *y)
{
  GeglCurvePrivate *priv = GEGL_CURVE_GET_PRIVATE (GEGL_CURVE (self));
  GeglCurvePoint point;

  g_assert(index < priv->points->len);
  point = g_array_index(priv->points, GeglCurvePoint, index);

  *x = point.x;
  *y = point.y;
}

void
gegl_curve_set_point (GeglCurve      *self,
                     guint          index,
                     gfloat         x,
                     gfloat         y)
{
  GeglCurvePrivate *priv = GEGL_CURVE_GET_PRIVATE (GEGL_CURVE (self));
  GeglCurvePoint point = { x, y };

  g_assert(index < priv->points->len);
  g_array_index(priv->points, GeglCurvePoint, index) = point;

  priv->need_recalc = TRUE;
}

guint
gegl_curve_num_points (GeglCurve   *self)
{
  GeglCurvePrivate *priv = GEGL_CURVE_GET_PRIVATE (GEGL_CURVE (self));

  return priv->points->len;
}

static int
compare_point_indirs (const void *_p1, const void *_p2)
{
  GeglCurvePoint *p1 = *(GeglCurvePoint**)_p1;
  GeglCurvePoint *p2 = *(GeglCurvePoint**)_p2;

  if (p1->x < p2->x)
    return -1;
  if (p1->x > p2->x)
    return 1;
  return 0;
}

#define X(i)      (priv->indir[(i)]->x)
#define Y(i)      (priv->indir[(i)]->y)
#define Y2(i)     (priv->indir[(i)]->y2)
#define YCLAMP(y)         ((y)<priv->y_min ? priv->y_min : ((y)>priv->y_max ? priv->y_max : (y)))

static void
recalculate (GeglCurvePrivate *priv)
{
  guint len = priv->points->len;
  guint i;
  gint k;
  gfloat *b;

  if (!priv->need_recalc)
    return;

  priv->need_recalc = FALSE;

  if (len < 2)
    return;

  if (priv->indir != NULL)
    g_free(priv->indir);
  priv->indir = (GeglCurvePoint**)g_malloc(sizeof(GeglCurvePoint*) * len);

  for (i = 0; i < len; ++i)
    priv->indir[i] = &g_array_index(priv->points, GeglCurvePoint, i);

  qsort(priv->indir, len, sizeof(GeglCurvePoint*), compare_point_indirs);

  b = (gfloat*)g_malloc(sizeof(gfloat) * (len - 1));

  // lower natural boundary conditions
  Y2(0) = b[0] = 0.0;

  for (i = 1; i < len - 1; ++i)
  {
    gfloat sig = (X(i) - X(i-1)) / (X(i+1) - X(i-1));
    gfloat p = sig * Y2(i-1) + 2;

    Y2(i) = (sig - 1) / p;
    b[i] = ((Y(i+1) - Y(i))
           / (X(i+1) - X(i)) - (Y(i) - Y(i-1)) / (X(i) - X(i-1)));
    b[i] = (6 * b[i] / (X(i+1) - X(i-1)) - sig * b[i-1]) / p;
  }

  // upper natural boundary condition
  Y2(len-1) = 0.0;
  for (k = len - 2; k >= 0; --k)
    Y2(k) = Y2(k) * Y2(k+1) + b[k];

  /*
  for (i = 0; i < len; ++i)
  {
    printf("y2: %f   ", Y2(i))
  */

  g_free(b);
}

static guint
find_interval (GeglCurvePrivate *priv, gfloat u)
{
  guint len = priv->points->len;
  guint i = 0, j = len - 1;

  while (j - i > 1)
  {
    guint k = (i + j) / 2;
    if (X(k) > u)
      j = k;
    else
      i = k;
  }

  return i;
}

static gfloat
apply (GeglCurvePrivate *priv, gfloat u, guint i)
{
  gfloat h = X(i+1) - X(i);
  gfloat a = (X(i+1) - u) / h;
  gfloat b = (u - X(i)) / h;
  gfloat y = a*Y(i) + b*Y(i+1) + ((a*a*a-a)*Y2(i) + (b*b*b-b)*Y2(i+1)) * (h*h)/6;

  return YCLAMP(y);
}

gfloat
gegl_curve_calc_value (GeglCurve   *self,
                      gfloat      x)
{
  GeglCurvePrivate *priv = GEGL_CURVE_GET_PRIVATE (GEGL_CURVE (self));

  recalculate(priv);

  if (priv->points->len >= 2)
    return apply(priv, x, find_interval(priv, x));
  else if (priv->points->len == 1)
    return YCLAMP(g_array_index(priv->points, GeglCurvePoint, 0).y);
  else
  {
    g_assert(priv->points->len == 0);
    return priv->y_min;
  }
}

void
gegl_curve_calc_values (GeglCurve   *self,
                       gfloat      x_min,
                       gfloat      x_max,
                       guint       num_samples,
                       gfloat      *xs,
                       gfloat      *ys)
{
  GeglCurvePrivate *priv = GEGL_CURVE_GET_PRIVATE (GEGL_CURVE (self));
  guint len = priv->points->len;
  guint i, j;

  recalculate(priv);

  j = 0;
  for (i = 0; i < num_samples; ++i)
  {
    gfloat u = x_min + (x_max - x_min) * (double)i / (double)(num_samples - 1);

    xs[i] = u;

    if (len >= 2)
    {
      while (j < len - 2 && X(j+1) < u)
       ++j;

      ys[i] = apply(priv, u, j);
    }
    else if (len == 1)
      ys[i] = YCLAMP(g_array_index(priv->points, GeglCurvePoint, 0).y);
    else
    {
      g_assert(len == 0);
      ys[i] = priv->y_min;
    }
  }
}

#undef X
#undef Y
#undef Y2
#undef YCLAMP

/* --------------------------------------------------------------------------
 * A GParamSpec class to describe behavior of GeglCurve as an object property
 * follows.
 * --------------------------------------------------------------------------
 */

#define GEGL_TYPE_PARAM_CURVE               (gegl_param_curve_get_type ())
#define GEGL_PARAM_CURVE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GEGL_TYPE_PARAM_CURVE, GeglParamCurve))
#define GEGL_IS_PARAM_CURVE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GEGL_TYPE_PARAM_CURVE))
#define GEGL_IS_PARAM_CURVE_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), GEGL_TYPE_PARAM_CURVE))

typedef struct _GeglParamCurve GeglParamCurve;

struct _GeglParamCurve
{
  GParamSpec parent_instance;

  GeglCurve *default_curve;
};

static void
gegl_param_curve_init (GParamSpec *self)
{
  GEGL_PARAM_CURVE (self)->default_curve = NULL;
}

static void
gegl_param_curve_finalize (GParamSpec *self)
{
  GeglParamCurve  *param_curve  = GEGL_PARAM_CURVE (self);
  GParamSpecClass *parent_class = g_type_class_peek (g_type_parent (GEGL_TYPE_PARAM_CURVE));

  if (param_curve->default_curve)
    {
      g_object_unref (param_curve->default_curve);
      param_curve->default_curve = NULL;
    }

  parent_class->finalize (self);
}

static void
value_set_default (GParamSpec *param_spec,
                   GValue     *value)
{
  GeglParamCurve *gegl_curve = GEGL_PARAM_CURVE (param_spec);

  g_object_ref (gegl_curve->default_curve); /* XXX:
                                               not sure why this is needed,
                                               but a reference is leaked
                                               unless it his here */
  g_value_set_object (value, gegl_curve->default_curve);
}

GType
gegl_param_curve_get_type (void)
{
  static GType param_curve_type = 0;

  if (G_UNLIKELY (param_curve_type == 0))
    {
      static GParamSpecTypeInfo param_curve_type_info = {
        sizeof (GeglParamCurve),
        0,
        gegl_param_curve_init,
        0,
        gegl_param_curve_finalize,
        value_set_default,
        NULL,
        NULL
      };
      param_curve_type_info.value_type = GEGL_TYPE_CURVE;

      param_curve_type = g_param_type_register_static ("GeglParamCurve",
                                                       &param_curve_type_info);
    }

  return param_curve_type;
}

GParamSpec *
gegl_param_spec_curve (const gchar *name,
                       const gchar *nick,
                       const gchar *blurb,
                       GeglCurve   *default_curve,
                       GParamFlags  flags)
{
  GeglParamCurve *param_curve;

  param_curve = g_param_spec_internal (GEGL_TYPE_PARAM_CURVE,
                                       name, nick, blurb, flags);

  param_curve->default_curve = default_curve;

  return G_PARAM_SPEC (param_curve);
}
