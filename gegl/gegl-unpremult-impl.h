#ifndef __GEGL_UNPREMULT_IMPL_H__
#define __GEGL_UNPREMULT_IMPL_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "gegl-point-op-impl.h"

#define GEGL_TYPE_UNPREMULT_IMPL               (gegl_unpremult_impl_get_type ())
#define GEGL_UNPREMULT_IMPL(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GEGL_TYPE_UNPREMULT_IMPL, GeglUnpremultImpl))
#define GEGL_UNPREMULT_IMPL_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass),  GEGL_TYPE_UNPREMULT_IMPL, GeglUnpremultImplClass))
#define GEGL_IS_UNPREMULT_IMPL(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GEGL_TYPE_UNPREMULT_IMPL))
#define GEGL_IS_UNPREMULT_IMPL_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass),  GEGL_TYPE_UNPREMULT_IMPL))
#define GEGL_UNPREMULT_IMPL_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj),  GEGL_TYPE_UNPREMULT_IMPL, GeglUnpremultImplClass))

#ifndef __TYPEDEF_GEGL_UNPREMULT_IMPL__
#define __TYPEDEF_GEGL_UNPREMULT_IMPL__
typedef struct _GeglUnpremultImpl GeglUnpremultImpl;
#endif
struct _GeglUnpremultImpl {
   GeglPointOpImpl __parent__;

   /*< private >*/
};

typedef struct _GeglUnpremultImplClass GeglUnpremultImplClass;
struct _GeglUnpremultImplClass {
   GeglPointOpImplClass __parent__;
};

GType           gegl_unpremult_impl_get_type       (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
