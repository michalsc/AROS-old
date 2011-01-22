/*
 * Mesa 3-D graphics library
 *
 * Copyright (C) 2010  VMware, Inc.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include "context.h"
#include "version.h"



/**
 * Examine enabled GL extensions to determine GL version.
 * Return major and minor version numbers.
 */
static void
compute_version(struct gl_context *ctx)
{
   GLuint major, minor;
   static const int max = 100;

   const GLboolean ver_1_3 = (ctx->Extensions.ARB_multisample &&
                              ctx->Extensions.ARB_multitexture &&
                              ctx->Extensions.ARB_texture_border_clamp &&
                              ctx->Extensions.ARB_texture_compression &&
                              ctx->Extensions.ARB_texture_cube_map &&
                              ctx->Extensions.EXT_texture_env_add &&
                              ctx->Extensions.ARB_texture_env_combine &&
                              ctx->Extensions.ARB_texture_env_dot3);
   const GLboolean ver_1_4 = (ver_1_3 &&
                              ctx->Extensions.ARB_depth_texture &&
                              ctx->Extensions.ARB_shadow &&
                              ctx->Extensions.ARB_texture_env_crossbar &&
                              ctx->Extensions.ARB_texture_mirrored_repeat &&
                              ctx->Extensions.ARB_window_pos &&
                              ctx->Extensions.EXT_blend_color &&
                              ctx->Extensions.EXT_blend_func_separate &&
                              ctx->Extensions.EXT_blend_minmax &&
                              ctx->Extensions.EXT_blend_subtract &&
                              ctx->Extensions.EXT_fog_coord &&
                              ctx->Extensions.EXT_multi_draw_arrays &&
                              ctx->Extensions.EXT_point_parameters &&
                              ctx->Extensions.EXT_secondary_color &&
                              ctx->Extensions.EXT_stencil_wrap &&
                              ctx->Extensions.EXT_texture_lod_bias &&
                              ctx->Extensions.SGIS_generate_mipmap);
   const GLboolean ver_1_5 = (ver_1_4 &&
                              ctx->Extensions.ARB_occlusion_query &&
                              ctx->Extensions.ARB_vertex_buffer_object &&
                              ctx->Extensions.EXT_shadow_funcs);
   const GLboolean ver_2_0 = (ver_1_5 &&
                              ctx->Extensions.ARB_draw_buffers &&
                              ctx->Extensions.ARB_point_sprite &&
                              ctx->Extensions.ARB_shader_objects &&
                              ctx->Extensions.ARB_vertex_shader &&
                              ctx->Extensions.ARB_fragment_shader &&
                              ctx->Extensions.ARB_texture_non_power_of_two &&
                              ctx->Extensions.EXT_blend_equation_separate &&

			      /* Technically, 2.0 requires the functionality
			       * of the EXT version.  Enable 2.0 if either
			       * extension is available, and assume that a
			       * driver that only exposes the ATI extension
			       * will fallback to software when necessary.
			       */
			      (ctx->Extensions.EXT_stencil_two_side
			       || ctx->Extensions.ATI_separate_stencil));
   const GLboolean ver_2_1 = (ver_2_0 &&
                              ctx->Const.GLSLVersion >= 120 &&
                              ctx->Extensions.EXT_pixel_buffer_object &&
                              ctx->Extensions.EXT_texture_sRGB);
   const GLboolean ver_3_0 = (ver_2_1 &&
                              ctx->Extensions.ARB_half_float_pixel &&
                              ctx->Extensions.ARB_map_buffer_range &&
                              ctx->Extensions.ARB_texture_float &&
                              ctx->Extensions.ARB_texture_rg &&
                              ctx->Extensions.ARB_texture_compression_rgtc &&
                              ctx->Extensions.APPLE_vertex_array_object &&
                              ctx->Extensions.EXT_draw_buffers2 &&
                              ctx->Extensions.EXT_framebuffer_blit &&
                              ctx->Extensions.EXT_framebuffer_multisample &&
                              ctx->Extensions.EXT_framebuffer_object &&
                              ctx->Extensions.EXT_framebuffer_sRGB &&
                              ctx->Extensions.EXT_packed_depth_stencil &&
                              ctx->Extensions.EXT_packed_float &&
                              ctx->Extensions.EXT_texture_array &&
                              ctx->Extensions.EXT_texture_integer &&
                              ctx->Extensions.EXT_texture_shared_exponent &&
                              ctx->Extensions.EXT_transform_feedback &&
                              ctx->Extensions.NV_conditional_render);
   const GLboolean ver_3_1 = (ver_3_0 &&
                              ctx->Extensions.ARB_copy_buffer &&
                              ctx->Extensions.ARB_draw_instanced &&
                              ctx->Extensions.ARB_texture_buffer_object &&
                              ctx->Extensions.ARB_uniform_buffer_object &&
                              ctx->Extensions.NV_primitive_restart &&
                              ctx->Extensions.NV_texture_rectangle &&
                              ctx->Const.MaxVertexTextureImageUnits >= 16);
   const GLboolean ver_3_2 = (ver_3_1 &&
                              ctx->Extensions.ARB_depth_clamp &&
                              ctx->Extensions.ARB_draw_elements_base_vertex &&
                              ctx->Extensions.ARB_fragment_coord_conventions &&
                              ctx->Extensions.ARB_geometry_shader4 &&
                              ctx->Extensions.EXT_provoking_vertex &&
                              ctx->Extensions.ARB_seamless_cube_map &&
                              ctx->Extensions.ARB_sync &&
                              ctx->Extensions.ARB_texture_multisample &&
                              ctx->Extensions.EXT_vertex_array_bgra);
   const GLboolean ver_3_3 = (ver_3_2 &&
                              ctx->Extensions.ARB_blend_func_extended &&
                              ctx->Extensions.ARB_explicit_attrib_location &&
                              ctx->Extensions.ARB_instanced_arrays &&
                              ctx->Extensions.ARB_occlusion_query2 &&
                              ctx->Extensions.ARB_sampler_objects &&
                              ctx->Extensions.ARB_texture_rgb10_a2ui &&
                              ctx->Extensions.ARB_timer_query &&
                              ctx->Extensions.ARB_vertex_type_2_10_10_10_rev &&
                              ctx->Extensions.EXT_texture_swizzle);

   if (ver_3_3) {
      major = 3;
      minor = 3;
   }
   else if (ver_3_2) {
      major = 3;
      minor = 2;
   }
   else if (ver_3_1) {
      major = 3;
      minor = 1;
   }
   else if (ver_3_0) {
      major = 3;
      minor = 0;
   }
   else if (ver_2_1) {
      major = 2;
      minor = 1;
   }
   else if (ver_2_0) {
      major = 2;
      minor = 0;
   }
   else if (ver_1_5) {
      major = 1;
      minor = 5;
   }
   else if (ver_1_4) {
      major = 1;
      minor = 4;
   }
   else if (ver_1_3) {
      major = 1;
      minor = 3;
   }
   else {
      major = 1;
      minor = 2;
   }

   ctx->VersionMajor = major;
   ctx->VersionMinor = minor;
   ctx->VersionString = (char *) malloc(max);
   if (ctx->VersionString) {
      _mesa_snprintf(ctx->VersionString, max,
		     "%u.%u Mesa " MESA_VERSION_STRING,
		     ctx->VersionMajor, ctx->VersionMinor);
   }
}

static void
compute_version_es1(struct gl_context *ctx)
{
   static const int max = 100;

   /* OpenGL ES 1.0 is derived from OpenGL 1.3 */
   const GLboolean ver_1_0 = (ctx->Extensions.ARB_multisample &&
                              ctx->Extensions.ARB_multitexture &&
                              ctx->Extensions.ARB_texture_compression &&
                              ctx->Extensions.EXT_texture_env_add &&
                              ctx->Extensions.ARB_texture_env_combine &&
                              ctx->Extensions.ARB_texture_env_dot3);
   /* OpenGL ES 1.1 is derived from OpenGL 1.5 */
   const GLboolean ver_1_1 = (ver_1_0 &&
                              ctx->Extensions.EXT_point_parameters &&
                              ctx->Extensions.SGIS_generate_mipmap &&
                              ctx->Extensions.ARB_vertex_buffer_object);

   if (ver_1_1) {
      ctx->VersionMajor = 1;
      ctx->VersionMinor = 1;
   } else if (ver_1_0) {
      ctx->VersionMajor = 1;
      ctx->VersionMinor = 0;
   } else {
      _mesa_problem(ctx, "Incomplete OpenGL ES 1.0 support.");
   }

   ctx->VersionString = (char *) malloc(max);
   if (ctx->VersionString) {
      _mesa_snprintf(ctx->VersionString, max,
		     "OpenGL ES-CM 1.%d Mesa " MESA_VERSION_STRING,
		     ctx->VersionMinor);
   }
}

static void
compute_version_es2(struct gl_context *ctx)
{
   static const int max = 100;

   /* OpenGL ES 2.0 is derived from OpenGL 2.0 */
   const GLboolean ver_2_0 = (ctx->Extensions.ARB_multisample &&
                              ctx->Extensions.ARB_multitexture &&
                              ctx->Extensions.ARB_texture_compression &&
                              ctx->Extensions.ARB_texture_cube_map &&
                              ctx->Extensions.ARB_texture_mirrored_repeat &&
                              ctx->Extensions.EXT_blend_color &&
                              ctx->Extensions.EXT_blend_func_separate &&
                              ctx->Extensions.EXT_blend_minmax &&
                              ctx->Extensions.EXT_blend_subtract &&
                              ctx->Extensions.EXT_stencil_wrap &&
                              ctx->Extensions.ARB_vertex_buffer_object &&
                              ctx->Extensions.ARB_shader_objects &&
                              ctx->Extensions.ARB_vertex_shader &&
                              ctx->Extensions.ARB_fragment_shader &&
                              ctx->Extensions.ARB_texture_non_power_of_two &&
                              ctx->Extensions.EXT_blend_equation_separate);
   if (ver_2_0) {
      ctx->VersionMajor = 2;
      ctx->VersionMinor = 0;
   } else {
      _mesa_problem(ctx, "Incomplete OpenGL ES 2.0 support.");
   }

   ctx->VersionString = (char *) malloc(max);
   if (ctx->VersionString) {
      _mesa_snprintf(ctx->VersionString, max,
		     "OpenGL ES 2.0 Mesa " MESA_VERSION_STRING);
   }
}

/**
 * Set the context's VersionMajor, VersionMinor, VersionString fields.
 * This should only be called once as part of context initialization
 * or to perform version check for GLX_ARB_create_context_profile.
 */
void
_mesa_compute_version(struct gl_context *ctx)
{
   if (ctx->VersionMajor)
      return;

   switch (ctx->API) {
   case API_OPENGL:
      compute_version(ctx);
      break;
   case API_OPENGLES:
      compute_version_es1(ctx);
      break;
   case API_OPENGLES2:
      compute_version_es2(ctx);
      break;
   }

}
