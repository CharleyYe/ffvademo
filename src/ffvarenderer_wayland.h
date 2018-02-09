/*
 * ffvarenderer_wayland.h - VA/WAYLAND renderer
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

#ifndef FFVA_RENDERER_WAYLAND_H
#define FFVA_RENDERER_WAYLAND_H

#include "ffvarenderer.h"

#define FFVA_RENDERER_WAYLAND(rnd) \
    ((FFVARendererWAYLAND *)(rnd))

typedef struct ffva_renderer_wayland_s      FFVARendererWAYLAND;

/** Creates a new renderer object from the supplied VA display */
FFVARenderer *
ffva_renderer_wayland_new(FFVADisplay *display, uint32_t flags);

#endif /* FFVA_RENDERER_WAYLAND_H */
