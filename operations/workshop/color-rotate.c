/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * This is a plug-in for GIMP.
 *
 * Colormap-Rotation plug-in. Exchanges two color ranges.
 *
 * Copyright (C) 1999 Sven Anders (anderss@fmi.uni-passau.de)
 *                    Based on code from Pavel Grinfeld (pavel@ml.com)
 * Copyright (C) 2011 Robert Sasu <sasu.robert@gmail.com>
 * Copyright (C) 2011 Mukund Sivaraman <muks@banu.com>
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

property_boolean (s_cl, _("Clockwise"), FALSE)
    description (_("Switch to clockwise"))

property_int (s_fr, _("From"), 0)
    description (_("Starting angle for the color rotation"))
    value_range (0, 360)
    ui_meta     ("unit", "degree")

property_int (s_to, _("To"), 0)
    description (_("End angle for the color rotation"))
    value_range (0, 360)
    ui_meta     ("unit", "degree")

property_boolean (d_cl, _("Clockwise"), FALSE)
    description (_("Switch to clockwise"))

property_int (d_fr, _("From"), 0)
    value_range (0, 360)
    description (_("Starting angle for the color rotation"))

property_int (d_to, _("To"), 0)
    value_range (0, 360)
    description (_("End angle for the color rotation"))

property_boolean (gray, _("Grayscale"), FALSE)
    description (_("Choose in case of grayscale images"))

property_double (hue, _("Hue"), 0.0)
    value_range (0.0, 2.0)

property_double (saturation, _("Saturation"), 0.0)
    value_range (0.0, 1.0)

property_boolean (change, _("Change/treat to this"), FALSE)

property_double (threshold, _("Gray threshold"), 0.0)
    value_range (0.0, 1.0)

#else

#define GEGL_OP_FILTER
#define GEGL_OP_C_FILE       "color-rotate.c"

#include "gegl-op.h"
#include <stdio.h>
#include <math.h>

#define TWO_PI (2 * G_PI)
#define DEG_TO_RAD(d) (((d) * G_PI) / 180.0)

static void prepare (GeglOperation *operation)
{
  gegl_operation_set_format (operation, "input",
                             babl_format ("RGBA float"));

  gegl_operation_set_format (operation, "output",
                             babl_format ("RGBA float"));
}

static void
rgb_to_hsv (gfloat  r,
            gfloat  g,
            gfloat  b,
            gfloat *h,
            gfloat *s,
            gfloat *v)
{
  float min;
  float delta;

  *v = MAX (MAX (r, g), b);
  min = MIN (MIN (r, g), b);
  delta = *v - min;

  if (delta == 0.0f)
    {
      *h = 0.0f;
      *s = 0.0f;
    }
  else
    {
      *s = delta / *v;

      if (r == *v)
        {
          *h = (g - b) / delta;
          if (*h < 0.0f)
            *h += 6.0f;
        }
      else if (g == *v)
        {
          *h = 2.0f + (b - r) / delta;
        }
      else
        {
          *h = 4.0f + (r - g) / delta;
        }

      *h /= 6.0f;
    }
}

static void
hsv_to_rgb (gfloat  h,
            gfloat  s,
            gfloat  v,
            gfloat *r,
            gfloat *g,
            gfloat *b)
{
  if (s == 0.0)
    {
      *r = v;
      *g = v;
      *b = v;
    }
  else
    {
      int hi;
      float frac;
      float w, q, t;

      h *= 6.0;

      if (h >= 6.0)
        h -= 6.0;

      hi = (int) h;
      frac = h - hi;
      w = v * (1.0 - s);
      q = v * (1.0 - (s * frac));
      t = v * (1.0 - (s * (1.0 - frac)));

      switch (hi)
        {
        case 0:
          *r = v;
          *g = t;
          *b = w;
          break;
        case 1:
          *r = q;
          *g = v;
          *b = w;
          break;
        case 2:
          *r = w;
          *g = v;
          *b = t;
          break;
        case 3:
          *r = w;
          *g = q;
          *b = v;
          break;
        case 4:
          *r = t;
          *g = w;
          *b = v;
          break;
        case 5:
          *r = v;
          *g = w;
          *b = q;
          break;
        }
    }
}

static gfloat
angle_mod_2PI (gfloat angle)
{
  if (angle < 0)
    return angle + TWO_PI;
  else if (angle > TWO_PI)
    return angle - TWO_PI;
  else
    return angle;
}

static gfloat
angle_inside_slice (gfloat   hue,
                    gint     from,
                    gint     to,
                    gboolean cl)
{
  gint cw_ccw = 1;
  if (!cl) cw_ccw = -1;

  return angle_mod_2PI (cw_ccw * DEG_TO_RAD(to - hue)) /
    angle_mod_2PI (cw_ccw * DEG_TO_RAD(from - to));
}

static gboolean
is_gray (gfloat  s,
         gdouble threshold)
{
  return (s <= threshold);
}

static gfloat
linear (gfloat A,
        gfloat B,
        gfloat C,
        gfloat D,
        gfloat x)
{
  if (B > A)
    {
      if (A<=x && x<=B)
        return C+(D-C)/(B-A)*(x-A);
      else if (A<=x+TWO_PI && x+TWO_PI<=B)
        return C+(D-C)/(B-A)*(x+TWO_PI-A);
      else
        return x;
    }
  else
    {
      if (B<=x && x<=A)
        return C+(D-C)/(B-A)*(x-A);
      else if (B<=x+TWO_PI && x+TWO_PI<=A)
        return C+(D-C)/(B-A)*(x+TWO_PI-A);
      else
        return x;
    }
}

static gfloat
left_end (gint     from,
          gint     to,
          gboolean cl)
{
  gfloat alpha  = DEG_TO_RAD (from);
  gfloat beta   = DEG_TO_RAD (to);
  gint   cw_ccw = cl ? 1 : -1;

  switch (cw_ccw)
    {
    case (-1):
      if (alpha < beta) return alpha + TWO_PI;

    default:
      return alpha; /* 1 */
    }
}

static gfloat
right_end (gint     from,
           gint     to,
           gboolean cl)
{
  gfloat alpha  = DEG_TO_RAD (from);
  gfloat beta   = DEG_TO_RAD (to);
  gint   cw_ccw = cl ? 1 : -1;

  switch (cw_ccw)
    {
    case 1:
      if (beta < alpha) return beta + TWO_PI;

    default:
      return beta; /* -1 */
    }
}

static void
color_rotate (gfloat     *src,
              gint        offset,
              GeglProperties *o)
{
  gfloat   h, s, v;
  gboolean skip = FALSE;
  gfloat   color[4];
  gint     i;

  rgb_to_hsv (src[offset],
              src[offset + 1],
              src[offset + 2],
              &h, &s, &v);

  if (is_gray (s, o->threshold))
    {
      if (o->change == FALSE)
        {
          if (angle_inside_slice (o->hue, o->s_fr, o->s_to, o->s_cl) <= 1)
            {
              h = o->hue / TWO_PI;
              s = o->saturation;
            }
          else
            {
              skip = TRUE;
            }
        }
      else
        {
          skip = TRUE;
          hsv_to_rgb (o->hue / TWO_PI, o->saturation, v,
                      color, color + 1, color + 2);
          color[3] = src[offset + 3];
        }
    }

  if (! skip)
    {
      h = linear (left_end (o->s_fr, o->s_to, o->s_cl),
                  right_end (o->s_fr, o->s_to, o->s_cl),
                  left_end (o->d_fr, o->d_to, o->d_cl),
                  right_end (o->d_fr, o->d_to, o->d_cl),
                  h * TWO_PI);
      h = angle_mod_2PI (h) / TWO_PI;
      hsv_to_rgb (h, s, v,
                  color, color + 1, color + 2);
      color[3] = src[offset + 3];
    }

  for (i = 0; i < 4; i++)
    src[offset + i] = color[i];
}


static gboolean
process (GeglOperation       *operation,
         GeglBuffer          *input,
         GeglBuffer          *output,
         const GeglRectangle *result,
         gint                 level)
{
  GeglProperties *o      = GEGL_PROPERTIES (operation);
  const Babl *format = babl_format ("RGBA float");
  gfloat     *src_buf;
  gint        x;

  src_buf = g_new0 (gfloat, result->width * result->height * 4);

  gegl_buffer_get (input, result, 1.0, format, src_buf,
                   GEGL_AUTO_ROWSTRIDE, GEGL_ABYSS_NONE);

  for (x = 0; x < result->width * result->height; x++)
    color_rotate (src_buf, 4 * x, o);

  gegl_buffer_set (output, result, 0, format, src_buf, GEGL_AUTO_ROWSTRIDE);

  g_free (src_buf);

  return  TRUE;
}


static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass       *operation_class;
  GeglOperationFilterClass *filter_class;

  operation_class = GEGL_OPERATION_CLASS (klass);
  filter_class    = GEGL_OPERATION_FILTER_CLASS (klass);

  filter_class->process    = process;
  operation_class->prepare = prepare;

  gegl_operation_class_set_keys (operation_class,
    "categories"  , "color",
    "name"        , "gegl:color-rotate",
    "description" , _("Rotate colors on the image"),
    NULL);
}

#endif
