
/* !!!! AUTOGENERATED FILE generated by svg-12-porter-duff.rb !!!!!
 *
 * This file is an image processing operation for GEGL
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
 *  Copyright 2006, 2007 Øyvind Kolås <pippin@gimp.org>
 *            2007 John Marshall
 *
 * SVG rendering modes; see:
 *     http://www.w3.org/TR/SVG12/rendering.html
 *     http://www.w3.org/TR/2004/WD-SVG12-20041027/rendering.html#comp-op-prop
 *
 *     aA = aux(src) alpha      aB = in(dst) alpha      aD = out alpha
 *     cA = aux(src) colour     cB = in(dst) colour     cD = out colour
 *
 * !!!! AUTOGENERATED FILE !!!!!
 */
#include "config.h"
#include <glib/gi18n-lib.h>


#ifdef GEGL_CHANT_PROPERTIES

/* no properties */

#else

#define GEGL_CHANT_TYPE_POINT_COMPOSER
#define GEGL_CHANT_C_FILE        "src-out.c"

#include "gegl-chant.h"

static void prepare (GeglOperation *operation)
{
  const Babl *format = babl_format ("RaGaBaA float");

  gegl_operation_set_format (operation, "input", format);
  gegl_operation_set_format (operation, "aux", format);
  gegl_operation_set_format (operation, "output", format);
}

static gboolean
process (GeglOperation        *op,
          void                *in_buf,
          void                *aux_buf,
          void                *out_buf,
          glong                n_pixels,
          const GeglRectangle *roi,
          gint                 level)
{
  gint i;
  gfloat * GEGL_ALIGNED in = in_buf;
  gfloat * GEGL_ALIGNED aux = aux_buf;
  gfloat * GEGL_ALIGNED out = out_buf;

  if (!aux)
    return TRUE;
  else
    {
      for (i = 0; i < n_pixels; i++)
        {
          gint   j;
          gfloat aA G_GNUC_UNUSED, aB G_GNUC_UNUSED, aD G_GNUC_UNUSED;

          aB = in[3];
          aA = aux[3];
          aD = aA * (1.0f - aB);

          for (j = 0; j < 3; j++)
            {
              gfloat cA G_GNUC_UNUSED, cB G_GNUC_UNUSED;

              cB = in[j];
              cA = aux[j];
              out[j] = cA * (1.0f - aB);
            }
          out[3] = aD;
          in  += 4;
          aux += 4;
          out += 4;
        }
    }
  return TRUE;
}


static void
gegl_chant_class_init (GeglChantClass *klass)
{
  GeglOperationClass              *operation_class;
  GeglOperationPointComposerClass *point_composer_class;

  operation_class      = GEGL_OPERATION_CLASS (klass);
  point_composer_class = GEGL_OPERATION_POINT_COMPOSER_CLASS (klass);

  point_composer_class->process = process;
  operation_class->prepare = prepare;


  gegl_operation_class_set_keys (operation_class,
    "name"       , "svg:src-out",
    "compat-name", "gegl:src-out",
    "categories" , "compositors:porter-duff",
    "description",
        _("Porter Duff operation src-out (d = cA * (1.0f - aB))"),
        NULL);
 

}

#endif
