#include "drmP.h"

int drm_dev_init(struct drm_device *dev, struct drm_driver *driver, struct device *parent)
{
    kref_init(&dev->ref);

    dev->dev = parent;
    dev->driver = driver;

    INIT_LIST_HEAD(&dev->filelist);
    INIT_LIST_HEAD(&dev->ctxlist);
    INIT_LIST_HEAD(&dev->vmalist);
    INIT_LIST_HEAD(&dev->maplist);
    INIT_LIST_HEAD(&dev->vblank_event_list);

    spin_lock_init(&dev->buf_lock);
    spin_lock_init(&dev->event_lock);
    mutex_init(&dev->struct_mutex);
    mutex_init(&dev->filelist_mutex);
    mutex_init(&dev->ctxlist_mutex);
    mutex_init(&dev->master_mutex);

    return drm_gem_init(dev);
}

struct drm_device *drm_dev_alloc(struct drm_driver *driver, struct device *parent)
{
    struct drm_device *dev;
    int ret;

    dev = kzalloc(sizeof(*dev), GFP_KERNEL);

    if (!dev)
        return ERR_PTR(-ENOMEM);
    
    ret = drm_dev_init(dev, driver, parent);
    if (ret) {
        kfree(dev);
        return ERR_PTR(ret);
    }

    return dev;
}

