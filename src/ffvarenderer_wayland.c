/*
 * ffvarenderer_waylanc.c - VA/WAYLAND renderer
 *
 * Copyright (C) 2018 Sony Corporation
 *   Author: Chenglin Ye <chenglin.ye@sony.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301
 */

#include "sysdeps.h"
#include <wayland-client.h>
#include <wayland-egl.h>
#include <va/va_wayland.h>
#include "ffvarenderer_wayland.h"
#include "ffvarenderer_priv.h"
#include "ffvadisplay_priv.h"

struct ffva_renderer_wayland_s {
    FFVARenderer base;

	struct wl_display *display;
	struct wl_registry *registry;
	struct wl_compositor *compositor;
	struct wl_shell *shell;
	struct wl_surface *surface;
	struct wl_shell_surface *shell_surface;
	struct wl_egl_window *window;
	struct wl_buffer *buffer;

	uint32_t display_width;
    uint32_t display_height;
};

/* Check VA status for success or print out an error */
static bool
vaapi_check_status (VAStatus status, const char * msg)
{
  if (status != VA_STATUS_SUCCESS) {
    return false;
  }
  return true;
}

static bool
create_window(FFVARendererWAYLAND *rnd, uint32_t width, uint32_t height)
{
	/* Create wl surface */
	rnd->surface = wl_compositor_create_surface(rnd->compositor);
	if (!rnd->surface) {
	    av_log(rnd, AV_LOG_ERROR, "failed to create wl surface\n");
        return false;
	}

	/* Create shell surface */
	rnd->shell_surface = wl_shell_get_shell_surface (rnd->shell, rnd->surface);
	if (!rnd->surface) {
	    av_log(rnd, AV_LOG_ERROR, "failed to create shell surface\n");
        return false;
	}
//	wl_shell_surface_add_listener (rnd->shell_surface, &shell_surface_listener, window);
	wl_shell_surface_set_toplevel (rnd->shell_surface);

	rnd->window =
		wl_egl_window_create(rnd->surface, width, height);

	rnd->display_width = width;
	rnd->display_height = height;
    rnd->base.window = (void *)(uintptr_t)rnd->window;

	return true;
}

static void
registry_handle_global(void *data, struct wl_registry *registry,
		       uint32_t id, const char *interface, uint32_t version)
{
	FFVARendererWAYLAND *rnd = data;

	if (strcmp(interface, "wl_compositor") == 0) {
		rnd->compositor =
			wl_registry_bind(registry,
					 id, &wl_compositor_interface, 1);
	} else if (strcmp(interface, "wl_shell") == 0) {
		rnd->shell =
			wl_registry_bind(registry, id,
					 &wl_shell_interface, 1);
	}
}

static void
registry_handle_global_remove(void *data, struct wl_registry *registry,
			      uint32_t name)
{
}

static const struct wl_registry_listener registry_listener = {
	registry_handle_global,
	registry_handle_global_remove
};

static bool
renderer_init(FFVARendererWAYLAND *rnd, uint32_t flags)
{
    FFVADisplay * const display = rnd->base.display;

    if (ffva_display_get_type(display) != FFVA_DISPLAY_TYPE_WAYLAND)
        return false;

	rnd->display = display->native_display;

	rnd->registry = wl_display_get_registry(rnd->display);
	wl_registry_add_listener(rnd->registry,
				 &registry_listener, rnd);
	wl_display_roundtrip(rnd->display);

    return true;
}

static void
renderer_finalize(FFVARendererWAYLAND *rnd)
{
//    wl_egl_window_destroy(rnd->window);
    wl_shell_surface_destroy(rnd->shell_surface);
	wl_surface_destroy(rnd->surface);
	wl_compositor_destroy(rnd->compositor);
	wl_registry_destroy(rnd->registry);
}

static bool
renderer_get_size(FFVARendererWAYLAND *rnd, uint32_t *width_ptr,
    uint32_t *height_ptr)
{
    if (width_ptr)
        *width_ptr = rnd->display_width;
    if (height_ptr)
        *height_ptr = rnd->display_height;
    return true;
}

static bool
renderer_set_size(FFVARendererWAYLAND *rnd, uint32_t width, uint32_t height)
{
	return create_window(rnd, width, height);
}

static bool
renderer_put_surface(FFVARendererWAYLAND *rnd, FFVASurface *surface,
    const VARectangle *src_rect, const VARectangle *dst_rect, uint32_t flags)
{
/*
	VAStatus status;

	status = vaGetSurfaceBufferWl(rnd->display, surface, VA_FRAME_PICTURE, rnd->buffer);
    if (!vaapi_check_status (status, "vaGetSurfaceBufferWl()"))
      return false;

	wl_surface_attach (rnd->surface, rnd->buffer, 0, 0);
    wl_surface_damage (rnd->surface, 0, 0, rnd->display_width, rnd->display_height);
	wl_surface_commit (rnd->surface);

	wl_buffer_destroy(rnd->buffer);
*/
    return true;
}

static const FFVARendererClass *
ffva_renderer_wayland_class(void)
{
    static const FFVARendererClass g_class = {
        .base = {
            .class_name = "FFVARendererWAYLAND",
            .item_name  = av_default_item_name,
            .option     = NULL,
            .version    = LIBAVUTIL_VERSION_INT,
        },
        .size           = sizeof(FFVARendererWAYLAND),
        .type           = FFVA_RENDERER_TYPE_WAYLAND,
        .init           = (FFVARendererInitFunc)renderer_init,
        .finalize       = (FFVARendererFinalizeFunc)renderer_finalize,
        .get_size       = (FFVARendererGetSizeFunc)renderer_get_size,
        .set_size       = (FFVARendererSetSizeFunc)renderer_set_size,
        .put_surface    = (FFVARendererPutSurfaceFunc)renderer_put_surface,
    };
    return &g_class;
}

// Creates a new renderer object from the supplied VA display
FFVARenderer *
ffva_renderer_wayland_new(FFVADisplay *display, uint32_t flags)
{
    return ffva_renderer_new(ffva_renderer_wayland_class(), display, flags);
}
