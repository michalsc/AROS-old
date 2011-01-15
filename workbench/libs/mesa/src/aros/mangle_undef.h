/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/


#undef glClearIndex
#undef glClearColor
#undef glClear
#undef glIndexMask
#undef glColorMask
#undef glAlphaFunc
#undef glBlendFunc
#undef glLogicOp
#undef glCullFace
#undef glFrontFace
#undef glPointSize
#undef glLineWidth
#undef glLineStipple
#undef glPolygonMode
#undef glPolygonOffset
#undef glPolygonStipple
#undef glGetPolygonStipple
#undef glEdgeFlag
#undef glEdgeFlagv
#undef glScissor
#undef glClipPlane
#undef glGetClipPlane
#undef glDrawBuffer
#undef glReadBuffer
#undef glEnable
#undef glDisable
#undef glIsEnabled
#undef glEnableClientState
#undef glDisableClientState
#undef glGetBooleanv
#undef glGetDoublev
#undef glGetFloatv
#undef glGetIntegerv
#undef glPushAttrib
#undef glPopAttrib
#undef glPushClientAttrib
#undef glPopClientAttrib
#undef glRenderMode
#undef glGetError
#undef glGetString
#undef glFinish
#undef glFlush
#undef glHint
#undef glClearDepth
#undef glDepthFunc
#undef glDepthMask
#undef glDepthRange
#undef glClearAccum
#undef glAccum
#undef glMatrixMode
#undef glOrtho
#undef glFrustum
#undef glViewport
#undef glPushMatrix
#undef glPopMatrix
#undef glLoadIdentity
#undef glLoadMatrixd
#undef glLoadMatrixf
#undef glMultMatrixd
#undef glMultMatrixf
#undef glRotated
#undef glRotatef
#undef glScaled
#undef glScalef
#undef glTranslated
#undef glTranslatef
#undef glIsList
#undef glDeleteLists
#undef glGenLists
#undef glNewList
#undef glEndList
#undef glCallList
#undef glCallLists
#undef glListBase
#undef glBegin
#undef glEnd
#undef glVertex2d
#undef glVertex2f
#undef glVertex2i
#undef glVertex2s
#undef glVertex3d
#undef glVertex3f
#undef glVertex3i
#undef glVertex3s
#undef glVertex4d
#undef glVertex4f
#undef glVertex4i
#undef glVertex4s
#undef glVertex2dv
#undef glVertex2fv
#undef glVertex2iv
#undef glVertex2sv
#undef glVertex3dv
#undef glVertex3fv
#undef glVertex3iv
#undef glVertex3sv
#undef glVertex4dv
#undef glVertex4fv
#undef glVertex4iv
#undef glVertex4sv
#undef glNormal3b
#undef glNormal3d
#undef glNormal3f
#undef glNormal3i
#undef glNormal3s
#undef glNormal3bv
#undef glNormal3dv
#undef glNormal3fv
#undef glNormal3iv
#undef glNormal3sv
#undef glIndexd
#undef glIndexf
#undef glIndexi
#undef glIndexs
#undef glIndexub
#undef glIndexdv
#undef glIndexfv
#undef glIndexiv
#undef glIndexsv
#undef glIndexubv
#undef glColor3b
#undef glColor3d
#undef glColor3f
#undef glColor3i
#undef glColor3s
#undef glColor3ub
#undef glColor3ui
#undef glColor3us
#undef glColor4b
#undef glColor4d
#undef glColor4f
#undef glColor4i
#undef glColor4s
#undef glColor4ub
#undef glColor4ui
#undef glColor4us
#undef glColor3bv
#undef glColor3dv
#undef glColor3fv
#undef glColor3iv
#undef glColor3sv
#undef glColor3ubv
#undef glColor3uiv
#undef glColor3usv
#undef glColor4bv
#undef glColor4dv
#undef glColor4fv
#undef glColor4iv
#undef glColor4sv
#undef glColor4ubv
#undef glColor4uiv
#undef glColor4usv
#undef glTexCoord1d
#undef glTexCoord1f
#undef glTexCoord1i
#undef glTexCoord1s
#undef glTexCoord2d
#undef glTexCoord2f
#undef glTexCoord2i
#undef glTexCoord2s
#undef glTexCoord3d
#undef glTexCoord3f
#undef glTexCoord3i
#undef glTexCoord3s
#undef glTexCoord4d
#undef glTexCoord4f
#undef glTexCoord4i
#undef glTexCoord4s
#undef glTexCoord1dv
#undef glTexCoord1fv
#undef glTexCoord1iv
#undef glTexCoord1sv
#undef glTexCoord2dv
#undef glTexCoord2fv
#undef glTexCoord2iv
#undef glTexCoord2sv
#undef glTexCoord3dv
#undef glTexCoord3fv
#undef glTexCoord3iv
#undef glTexCoord3sv
#undef glTexCoord4dv
#undef glTexCoord4fv
#undef glTexCoord4iv
#undef glTexCoord4sv
#undef glRasterPos2d
#undef glRasterPos2f
#undef glRasterPos2i
#undef glRasterPos2s
#undef glRasterPos3d
#undef glRasterPos3f
#undef glRasterPos3i
#undef glRasterPos3s
#undef glRasterPos4d
#undef glRasterPos4f
#undef glRasterPos4i
#undef glRasterPos4s
#undef glRasterPos2dv
#undef glRasterPos2fv
#undef glRasterPos2iv
#undef glRasterPos2sv
#undef glRasterPos3dv
#undef glRasterPos3fv
#undef glRasterPos3iv
#undef glRasterPos3sv
#undef glRasterPos4dv
#undef glRasterPos4fv
#undef glRasterPos4iv
#undef glRasterPos4sv
#undef glRectd
#undef glRectf
#undef glRecti
#undef glRects
#undef glRectdv
#undef glRectfv
#undef glRectiv
#undef glRectsv
#undef glVertexPointer
#undef glNormalPointer
#undef glColorPointer
#undef glIndexPointer
#undef glTexCoordPointer
#undef glEdgeFlagPointer
#undef glGetPointerv
#undef glArrayElement
#undef glDrawArrays
#undef glDrawElements
#undef glInterleavedArrays
#undef glShadeModel
#undef glLightf
#undef glLighti
#undef glLightfv
#undef glLightiv
#undef glGetLightfv
#undef glGetLightiv
#undef glLightModelf
#undef glLightModeli
#undef glLightModelfv
#undef glLightModeliv
#undef glMaterialf
#undef glMateriali
#undef glMaterialfv
#undef glMaterialiv
#undef glGetMaterialfv
#undef glGetMaterialiv
#undef glColorMaterial
#undef glPixelZoom
#undef glPixelStoref
#undef glPixelStorei
#undef glPixelTransferf
#undef glPixelTransferi
#undef glPixelMapfv
#undef glPixelMapuiv
#undef glPixelMapusv
#undef glGetPixelMapfv
#undef glGetPixelMapuiv
#undef glGetPixelMapusv
#undef glBitmap
#undef glReadPixels
#undef glDrawPixels
#undef glCopyPixels
#undef glStencilFunc
#undef glStencilMask
#undef glStencilOp
#undef glClearStencil
#undef glTexGend
#undef glTexGenf
#undef glTexGeni
#undef glTexGendv
#undef glTexGenfv
#undef glTexGeniv
#undef glGetTexGendv
#undef glGetTexGenfv
#undef glGetTexGeniv
#undef glTexEnvf
#undef glTexEnvi
#undef glTexEnvfv
#undef glTexEnviv
#undef glGetTexEnvfv
#undef glGetTexEnviv
#undef glTexParameterf
#undef glTexParameteri
#undef glTexParameterfv
#undef glTexParameteriv
#undef glGetTexParameterfv
#undef glGetTexParameteriv
#undef glGetTexLevelParameterfv
#undef glGetTexLevelParameteriv
#undef glTexImage1D
#undef glTexImage2D
#undef glGetTexImage
#undef glGenTextures
#undef glDeleteTextures
#undef glBindTexture
#undef glPrioritizeTextures
#undef glAreTexturesResident
#undef glIsTexture
#undef glTexSubImage1D
#undef glTexSubImage2D
#undef glCopyTexImage1D
#undef glCopyTexImage2D
#undef glCopyTexSubImage1D
#undef glCopyTexSubImage2D
#undef glMap1d
#undef glMap1f
#undef glMap2d
#undef glMap2f
#undef glGetMapdv
#undef glGetMapfv
#undef glGetMapiv
#undef glEvalCoord1d
#undef glEvalCoord1f
#undef glEvalCoord1dv
#undef glEvalCoord1fv
#undef glEvalCoord2d
#undef glEvalCoord2f
#undef glEvalCoord2dv
#undef glEvalCoord2fv
#undef glMapGrid1d
#undef glMapGrid1f
#undef glMapGrid2d
#undef glMapGrid2f
#undef glEvalPoint1
#undef glEvalPoint2
#undef glEvalMesh1
#undef glEvalMesh2
#undef glFogf
#undef glFogi
#undef glFogfv
#undef glFogiv
#undef glFeedbackBuffer
#undef glPassThrough
#undef glSelectBuffer
#undef glInitNames
#undef glLoadName
#undef glPushName
#undef glPopName
#undef glDrawRangeElements
#undef glTexImage3D
#undef glTexSubImage3D
#undef glCopyTexSubImage3D
#undef glColorTable
#undef glColorSubTable
#undef glColorTableParameteriv
#undef glColorTableParameterfv
#undef glCopyColorSubTable
#undef glCopyColorTable
#undef glGetColorTable
#undef glGetColorTableParameterfv
#undef glGetColorTableParameteriv
#undef glBlendEquation
#undef glBlendColor
#undef glHistogram
#undef glResetHistogram
#undef glGetHistogram
#undef glGetHistogramParameterfv
#undef glGetHistogramParameteriv
#undef glMinmax
#undef glResetMinmax
#undef glGetMinmax
#undef glGetMinmaxParameterfv
#undef glGetMinmaxParameteriv
#undef glConvolutionFilter1D
#undef glConvolutionFilter2D
#undef glConvolutionParameterf
#undef glConvolutionParameterfv
#undef glConvolutionParameteri
#undef glConvolutionParameteriv
#undef glCopyConvolutionFilter1D
#undef glCopyConvolutionFilter2D
#undef glGetConvolutionFilter
#undef glGetConvolutionParameterfv
#undef glGetConvolutionParameteriv
#undef glSeparableFilter2D
#undef glGetSeparableFilter
#undef glActiveTexture
#undef glClientActiveTexture
#undef glCompressedTexImage1D
#undef glCompressedTexImage2D
#undef glCompressedTexImage3D
#undef glCompressedTexSubImage1D
#undef glCompressedTexSubImage2D
#undef glCompressedTexSubImage3D
#undef glGetCompressedTexImage
#undef glMultiTexCoord1d
#undef glMultiTexCoord1dv
#undef glMultiTexCoord1f
#undef glMultiTexCoord1fv
#undef glMultiTexCoord1i
#undef glMultiTexCoord1iv
#undef glMultiTexCoord1s
#undef glMultiTexCoord1sv
#undef glMultiTexCoord2d
#undef glMultiTexCoord2dv
#undef glMultiTexCoord2f
#undef glMultiTexCoord2fv
#undef glMultiTexCoord2i
#undef glMultiTexCoord2iv
#undef glMultiTexCoord2s
#undef glMultiTexCoord2sv
#undef glMultiTexCoord3d
#undef glMultiTexCoord3dv
#undef glMultiTexCoord3f
#undef glMultiTexCoord3fv
#undef glMultiTexCoord3i
#undef glMultiTexCoord3iv
#undef glMultiTexCoord3s
#undef glMultiTexCoord3sv
#undef glMultiTexCoord4d
#undef glMultiTexCoord4dv
#undef glMultiTexCoord4f
#undef glMultiTexCoord4fv
#undef glMultiTexCoord4i
#undef glMultiTexCoord4iv
#undef glMultiTexCoord4s
#undef glMultiTexCoord4sv
#undef glLoadTransposeMatrixd
#undef glLoadTransposeMatrixf
#undef glMultTransposeMatrixd
#undef glMultTransposeMatrixf
#undef glSampleCoverage
#undef glActiveTextureARB
#undef glClientActiveTextureARB
#undef glMultiTexCoord1dARB
#undef glMultiTexCoord1dvARB
#undef glMultiTexCoord1fARB
#undef glMultiTexCoord1fvARB
#undef glMultiTexCoord1iARB
#undef glMultiTexCoord1ivARB
#undef glMultiTexCoord1sARB
#undef glMultiTexCoord1svARB
#undef glMultiTexCoord2dARB
#undef glMultiTexCoord2dvARB
#undef glMultiTexCoord2fARB
#undef glMultiTexCoord2fvARB
#undef glMultiTexCoord2iARB
#undef glMultiTexCoord2ivARB
#undef glMultiTexCoord2sARB
#undef glMultiTexCoord2svARB
#undef glMultiTexCoord3dARB
#undef glMultiTexCoord3dvARB
#undef glMultiTexCoord3fARB
#undef glMultiTexCoord3fvARB
#undef glMultiTexCoord3iARB
#undef glMultiTexCoord3ivARB
#undef glMultiTexCoord3sARB
#undef glMultiTexCoord3svARB
#undef glMultiTexCoord4dARB
#undef glMultiTexCoord4dvARB
#undef glMultiTexCoord4fARB
#undef glMultiTexCoord4fvARB
#undef glMultiTexCoord4iARB
#undef glMultiTexCoord4ivARB
#undef glMultiTexCoord4sARB
#undef glMultiTexCoord4svARB
#undef glBlendFuncSeparate
#undef glFogCoordf
#undef glFogCoordfv
#undef glFogCoordd
#undef glFogCoorddv
#undef glFogCoordPointer
#undef glMultiDrawArrays
#undef glMultiDrawElements
#undef glPointParameterf
#undef glPointParameterfv
#undef glPointParameteri
#undef glPointParameteriv
#undef glSecondaryColor3b
#undef glSecondaryColor3bv
#undef glSecondaryColor3d
#undef glSecondaryColor3dv
#undef glSecondaryColor3f
#undef glSecondaryColor3fv
#undef glSecondaryColor3i
#undef glSecondaryColor3iv
#undef glSecondaryColor3s
#undef glSecondaryColor3sv
#undef glSecondaryColor3ub
#undef glSecondaryColor3ubv
#undef glSecondaryColor3ui
#undef glSecondaryColor3uiv
#undef glSecondaryColor3us
#undef glSecondaryColor3usv
#undef glSecondaryColorPointer
#undef glWindowPos2d
#undef glWindowPos2dv
#undef glWindowPos2f
#undef glWindowPos2fv
#undef glWindowPos2i
#undef glWindowPos2iv
#undef glWindowPos2s
#undef glWindowPos2sv
#undef glWindowPos3d
#undef glWindowPos3dv
#undef glWindowPos3f
#undef glWindowPos3fv
#undef glWindowPos3i
#undef glWindowPos3iv
#undef glWindowPos3s
#undef glWindowPos3sv
#undef glGenQueries
#undef glDeleteQueries
#undef glIsQuery
#undef glBeginQuery
#undef glEndQuery
#undef glGetQueryiv
#undef glGetQueryObjectiv
#undef glGetQueryObjectuiv
#undef glBindBuffer
#undef glDeleteBuffers
#undef glGenBuffers
#undef glIsBuffer
#undef glBufferData
#undef glBufferSubData
#undef glGetBufferSubData
#undef glMapBuffer
#undef glUnmapBuffer
#undef glGetBufferParameteriv
#undef glGetBufferPointerv
#undef glBlendEquationSeparate
#undef glDrawBuffers
#undef glStencilOpSeparate
#undef glStencilFuncSeparate
#undef glStencilMaskSeparate
#undef glAttachShader
#undef glBindAttribLocation
#undef glCompileShader
#undef glCreateProgram
#undef glCreateShader
#undef glDeleteProgram
#undef glDeleteShader
#undef glDetachShader
#undef glDisableVertexAttribArray
#undef glEnableVertexAttribArray
#undef glGetActiveAttrib
#undef glGetActiveUniform
#undef glGetAttachedShaders
#undef glGetAttribLocation
#undef glGetProgramiv
#undef glGetProgramInfoLog
#undef glGetShaderiv
#undef glGetShaderInfoLog
#undef glGetShaderSource
#undef glGetUniformLocation
#undef glGetUniformfv
#undef glGetUniformiv
#undef glGetVertexAttribdv
#undef glGetVertexAttribfv
#undef glGetVertexAttribiv
#undef glGetVertexAttribPointerv
#undef glIsProgram
#undef glIsShader
#undef glLinkProgram
#undef glShaderSource
#undef glUseProgram
#undef glUniform1f
#undef glUniform2f
#undef glUniform3f
#undef glUniform4f
#undef glUniform1i
#undef glUniform2i
#undef glUniform3i
#undef glUniform4i
#undef glUniform1fv
#undef glUniform2fv
#undef glUniform3fv
#undef glUniform4fv
#undef glUniform1iv
#undef glUniform2iv
#undef glUniform3iv
#undef glUniform4iv
#undef glUniformMatrix2fv
#undef glUniformMatrix3fv
#undef glUniformMatrix4fv
#undef glValidateProgram
#undef glVertexAttrib1d
#undef glVertexAttrib1dv
#undef glVertexAttrib1f
#undef glVertexAttrib1fv
#undef glVertexAttrib1s
#undef glVertexAttrib1sv
#undef glVertexAttrib2d
#undef glVertexAttrib2dv
#undef glVertexAttrib2f
#undef glVertexAttrib2fv
#undef glVertexAttrib2s
#undef glVertexAttrib2sv
#undef glVertexAttrib3d
#undef glVertexAttrib3dv
#undef glVertexAttrib3f
#undef glVertexAttrib3fv
#undef glVertexAttrib3s
#undef glVertexAttrib3sv
#undef glVertexAttrib4Nbv
#undef glVertexAttrib4Niv
#undef glVertexAttrib4Nsv
#undef glVertexAttrib4Nub
#undef glVertexAttrib4Nubv
#undef glVertexAttrib4Nuiv
#undef glVertexAttrib4Nusv
#undef glVertexAttrib4bv
#undef glVertexAttrib4d
#undef glVertexAttrib4dv
#undef glVertexAttrib4f
#undef glVertexAttrib4fv
#undef glVertexAttrib4iv
#undef glVertexAttrib4s
#undef glVertexAttrib4sv
#undef glVertexAttrib4ubv
#undef glVertexAttrib4uiv
#undef glVertexAttrib4usv
#undef glVertexAttribPointer
#undef glUniformMatrix2x3fv
#undef glUniformMatrix3x2fv
#undef glUniformMatrix2x4fv
#undef glUniformMatrix4x2fv
#undef glUniformMatrix3x4fv
#undef glUniformMatrix4x3fv
#undef glLoadTransposeMatrixfARB
#undef glLoadTransposeMatrixdARB
#undef glMultTransposeMatrixfARB
#undef glMultTransposeMatrixdARB
#undef glSampleCoverageARB
#undef glCompressedTexImage3DARB
#undef glCompressedTexImage2DARB
#undef glCompressedTexImage1DARB
#undef glCompressedTexSubImage3DARB
#undef glCompressedTexSubImage2DARB
#undef glCompressedTexSubImage1DARB
#undef glGetCompressedTexImageARB
#undef glPointParameterfARB
#undef glPointParameterfvARB
#undef glWindowPos2dARB
#undef glWindowPos2dvARB
#undef glWindowPos2fARB
#undef glWindowPos2fvARB
#undef glWindowPos2iARB
#undef glWindowPos2ivARB
#undef glWindowPos2sARB
#undef glWindowPos2svARB
#undef glWindowPos3dARB
#undef glWindowPos3dvARB
#undef glWindowPos3fARB
#undef glWindowPos3fvARB
#undef glWindowPos3iARB
#undef glWindowPos3ivARB
#undef glWindowPos3sARB
#undef glWindowPos3svARB
#undef glVertexAttrib1dARB
#undef glVertexAttrib1dvARB
#undef glVertexAttrib1fARB
#undef glVertexAttrib1fvARB
#undef glVertexAttrib1sARB
#undef glVertexAttrib1svARB
#undef glVertexAttrib2dARB
#undef glVertexAttrib2dvARB
#undef glVertexAttrib2fARB
#undef glVertexAttrib2fvARB
#undef glVertexAttrib2sARB
#undef glVertexAttrib2svARB
#undef glVertexAttrib3dARB
#undef glVertexAttrib3dvARB
#undef glVertexAttrib3fARB
#undef glVertexAttrib3fvARB
#undef glVertexAttrib3sARB
#undef glVertexAttrib3svARB
#undef glVertexAttrib4NbvARB
#undef glVertexAttrib4NivARB
#undef glVertexAttrib4NsvARB
#undef glVertexAttrib4NubARB
#undef glVertexAttrib4NubvARB
#undef glVertexAttrib4NuivARB
#undef glVertexAttrib4NusvARB
#undef glVertexAttrib4bvARB
#undef glVertexAttrib4dARB
#undef glVertexAttrib4dvARB
#undef glVertexAttrib4fARB
#undef glVertexAttrib4fvARB
#undef glVertexAttrib4ivARB
#undef glVertexAttrib4sARB
#undef glVertexAttrib4svARB
#undef glVertexAttrib4ubvARB
#undef glVertexAttrib4uivARB
#undef glVertexAttrib4usvARB
#undef glVertexAttribPointerARB
#undef glEnableVertexAttribArrayARB
#undef glDisableVertexAttribArrayARB
#undef glProgramStringARB
#undef glBindProgramARB
#undef glDeleteProgramsARB
#undef glGenProgramsARB
#undef glProgramEnvParameter4dARB
#undef glProgramEnvParameter4dvARB
#undef glProgramEnvParameter4fARB
#undef glProgramEnvParameter4fvARB
#undef glProgramLocalParameter4dARB
#undef glProgramLocalParameter4dvARB
#undef glProgramLocalParameter4fARB
#undef glProgramLocalParameter4fvARB
#undef glGetProgramEnvParameterdvARB
#undef glGetProgramEnvParameterfvARB
#undef glGetProgramLocalParameterdvARB
#undef glGetProgramLocalParameterfvARB
#undef glGetProgramivARB
#undef glGetProgramStringARB
#undef glGetVertexAttribdvARB
#undef glGetVertexAttribfvARB
#undef glGetVertexAttribivARB
#undef glGetVertexAttribPointervARB
#undef glIsProgramARB
#undef glBindBufferARB
#undef glDeleteBuffersARB
#undef glGenBuffersARB
#undef glIsBufferARB
#undef glBufferDataARB
#undef glBufferSubDataARB
#undef glGetBufferSubDataARB
#undef glMapBufferARB
#undef glUnmapBufferARB
#undef glGetBufferParameterivARB
#undef glGetBufferPointervARB
#undef glGenQueriesARB
#undef glDeleteQueriesARB
#undef glIsQueryARB
#undef glBeginQueryARB
#undef glEndQueryARB
#undef glGetQueryivARB
#undef glGetQueryObjectivARB
#undef glGetQueryObjectuivARB
#undef glDeleteObjectARB
#undef glGetHandleARB
#undef glDetachObjectARB
#undef glCreateShaderObjectARB
#undef glShaderSourceARB
#undef glCompileShaderARB
#undef glCreateProgramObjectARB
#undef glAttachObjectARB
#undef glLinkProgramARB
#undef glUseProgramObjectARB
#undef glValidateProgramARB
#undef glUniform1fARB
#undef glUniform2fARB
#undef glUniform3fARB
#undef glUniform4fARB
#undef glUniform1iARB
#undef glUniform2iARB
#undef glUniform3iARB
#undef glUniform4iARB
#undef glUniform1fvARB
#undef glUniform2fvARB
#undef glUniform3fvARB
#undef glUniform4fvARB
#undef glUniform1ivARB
#undef glUniform2ivARB
#undef glUniform3ivARB
#undef glUniform4ivARB
#undef glUniformMatrix2fvARB
#undef glUniformMatrix3fvARB
#undef glUniformMatrix4fvARB
#undef glGetObjectParameterfvARB
#undef glGetObjectParameterivARB
#undef glGetInfoLogARB
#undef glGetAttachedObjectsARB
#undef glGetUniformLocationARB
#undef glGetActiveUniformARB
#undef glGetUniformfvARB
#undef glGetUniformivARB
#undef glGetShaderSourceARB
#undef glBindAttribLocationARB
#undef glGetActiveAttribARB
#undef glGetAttribLocationARB
#undef glDrawBuffersARB
#undef glIsRenderbuffer
#undef glBindRenderbuffer
#undef glDeleteRenderbuffers
#undef glGenRenderbuffers
#undef glRenderbufferStorage
#undef glGetRenderbufferParameteriv
#undef glIsFramebuffer
#undef glBindFramebuffer
#undef glDeleteFramebuffers
#undef glGenFramebuffers
#undef glCheckFramebufferStatus
#undef glFramebufferTexture1D
#undef glFramebufferTexture2D
#undef glFramebufferTexture3D
#undef glFramebufferRenderbuffer
#undef glGetFramebufferAttachmentParameteriv
#undef glGenerateMipmap
#undef glBlitFramebuffer
#undef glRenderbufferStorageMultisample
#undef glFramebufferTextureLayer
#undef glBlendColorEXT
#undef glPolygonOffsetEXT
#undef glTexImage3DEXT
#undef glTexSubImage3DEXT
#undef glTexSubImage1DEXT
#undef glTexSubImage2DEXT
#undef glCopyTexImage1DEXT
#undef glCopyTexImage2DEXT
#undef glCopyTexSubImage1DEXT
#undef glCopyTexSubImage2DEXT
#undef glCopyTexSubImage3DEXT
#undef glAreTexturesResidentEXT
#undef glBindTextureEXT
#undef glDeleteTexturesEXT
#undef glGenTexturesEXT
#undef glIsTextureEXT
#undef glPrioritizeTexturesEXT
#undef glArrayElementEXT
#undef glColorPointerEXT
#undef glDrawArraysEXT
#undef glEdgeFlagPointerEXT
#undef glGetPointervEXT
#undef glIndexPointerEXT
#undef glNormalPointerEXT
#undef glTexCoordPointerEXT
#undef glVertexPointerEXT
#undef glBlendEquationEXT
#undef glPointParameterfEXT
#undef glPointParameterfvEXT
#undef glColorTableEXT
#undef glGetColorTableEXT
#undef glGetColorTableParameterivEXT
#undef glGetColorTableParameterfvEXT
#undef glLockArraysEXT
#undef glUnlockArraysEXT
#undef glDrawRangeElementsEXT
#undef glSecondaryColor3bEXT
#undef glSecondaryColor3bvEXT
#undef glSecondaryColor3dEXT
#undef glSecondaryColor3dvEXT
#undef glSecondaryColor3fEXT
#undef glSecondaryColor3fvEXT
#undef glSecondaryColor3iEXT
#undef glSecondaryColor3ivEXT
#undef glSecondaryColor3sEXT
#undef glSecondaryColor3svEXT
#undef glSecondaryColor3ubEXT
#undef glSecondaryColor3ubvEXT
#undef glSecondaryColor3uiEXT
#undef glSecondaryColor3uivEXT
#undef glSecondaryColor3usEXT
#undef glSecondaryColor3usvEXT
#undef glSecondaryColorPointerEXT
#undef glMultiDrawArraysEXT
#undef glMultiDrawElementsEXT
#undef glFogCoordfEXT
#undef glFogCoordfvEXT
#undef glFogCoorddEXT
#undef glFogCoorddvEXT
#undef glFogCoordPointerEXT
#undef glBlendFuncSeparateEXT
#undef glFlushVertexArrayRangeNV
#undef glVertexArrayRangeNV
#undef glCombinerParameterfvNV
#undef glCombinerParameterfNV
#undef glCombinerParameterivNV
#undef glCombinerParameteriNV
#undef glCombinerInputNV
#undef glCombinerOutputNV
#undef glFinalCombinerInputNV
#undef glGetCombinerInputParameterfvNV
#undef glGetCombinerInputParameterivNV
#undef glGetCombinerOutputParameterfvNV
#undef glGetCombinerOutputParameterivNV
#undef glGetFinalCombinerInputParameterfvNV
#undef glGetFinalCombinerInputParameterivNV
#undef glResizeBuffersMESA
#undef glWindowPos2dMESA
#undef glWindowPos2dvMESA
#undef glWindowPos2fMESA
#undef glWindowPos2fvMESA
#undef glWindowPos2iMESA
#undef glWindowPos2ivMESA
#undef glWindowPos2sMESA
#undef glWindowPos2svMESA
#undef glWindowPos3dMESA
#undef glWindowPos3dvMESA
#undef glWindowPos3fMESA
#undef glWindowPos3fvMESA
#undef glWindowPos3iMESA
#undef glWindowPos3ivMESA
#undef glWindowPos3sMESA
#undef glWindowPos3svMESA
#undef glWindowPos4dMESA
#undef glWindowPos4dvMESA
#undef glWindowPos4fMESA
#undef glWindowPos4fvMESA
#undef glWindowPos4iMESA
#undef glWindowPos4ivMESA
#undef glWindowPos4sMESA
#undef glWindowPos4svMESA
#undef glAreProgramsResidentNV
#undef glBindProgramNV
#undef glDeleteProgramsNV
#undef glExecuteProgramNV
#undef glGenProgramsNV
#undef glGetProgramParameterdvNV
#undef glGetProgramParameterfvNV
#undef glGetProgramivNV
#undef glGetProgramStringNV
#undef glGetTrackMatrixivNV
#undef glGetVertexAttribdvNV
#undef glGetVertexAttribfvNV
#undef glGetVertexAttribivNV
#undef glGetVertexAttribPointervNV
#undef glIsProgramNV
#undef glLoadProgramNV
#undef glProgramParameter4dNV
#undef glProgramParameter4dvNV
#undef glProgramParameter4fNV
#undef glProgramParameter4fvNV
#undef glProgramParameters4dvNV
#undef glProgramParameters4fvNV
#undef glRequestResidentProgramsNV
#undef glTrackMatrixNV
#undef glVertexAttribPointerNV
#undef glVertexAttrib1dNV
#undef glVertexAttrib1dvNV
#undef glVertexAttrib1fNV
#undef glVertexAttrib1fvNV
#undef glVertexAttrib1sNV
#undef glVertexAttrib1svNV
#undef glVertexAttrib2dNV
#undef glVertexAttrib2dvNV
#undef glVertexAttrib2fNV
#undef glVertexAttrib2fvNV
#undef glVertexAttrib2sNV
#undef glVertexAttrib2svNV
#undef glVertexAttrib3dNV
#undef glVertexAttrib3dvNV
#undef glVertexAttrib3fNV
#undef glVertexAttrib3fvNV
#undef glVertexAttrib3sNV
#undef glVertexAttrib3svNV
#undef glVertexAttrib4dNV
#undef glVertexAttrib4dvNV
#undef glVertexAttrib4fNV
#undef glVertexAttrib4fvNV
#undef glVertexAttrib4sNV
#undef glVertexAttrib4svNV
#undef glVertexAttrib4ubNV
#undef glVertexAttrib4ubvNV
#undef glVertexAttribs1dvNV
#undef glVertexAttribs1fvNV
#undef glVertexAttribs1svNV
#undef glVertexAttribs2dvNV
#undef glVertexAttribs2fvNV
#undef glVertexAttribs2svNV
#undef glVertexAttribs3dvNV
#undef glVertexAttribs3fvNV
#undef glVertexAttribs3svNV
#undef glVertexAttribs4dvNV
#undef glVertexAttribs4fvNV
#undef glVertexAttribs4svNV
#undef glVertexAttribs4ubvNV
#undef glTexBumpParameterivATI
#undef glTexBumpParameterfvATI
#undef glGetTexBumpParameterivATI
#undef glGetTexBumpParameterfvATI
#undef glGenFragmentShadersATI
#undef glBindFragmentShaderATI
#undef glDeleteFragmentShaderATI
#undef glBeginFragmentShaderATI
#undef glEndFragmentShaderATI
#undef glPassTexCoordATI
#undef glSampleMapATI
#undef glColorFragmentOp1ATI
#undef glColorFragmentOp2ATI
#undef glColorFragmentOp3ATI
#undef glAlphaFragmentOp1ATI
#undef glAlphaFragmentOp2ATI
#undef glAlphaFragmentOp3ATI
#undef glSetFragmentShaderConstantATI
#undef glPointParameteriNV
#undef glPointParameterivNV
#undef glDrawBuffersATI
#undef glProgramNamedParameter4fNV
#undef glProgramNamedParameter4dNV
#undef glProgramNamedParameter4fvNV
#undef glProgramNamedParameter4dvNV
#undef glGetProgramNamedParameterfvNV
#undef glGetProgramNamedParameterdvNV
#undef glIsRenderbufferEXT
#undef glBindRenderbufferEXT
#undef glDeleteRenderbuffersEXT
#undef glGenRenderbuffersEXT
#undef glRenderbufferStorageEXT
#undef glGetRenderbufferParameterivEXT
#undef glIsFramebufferEXT
#undef glBindFramebufferEXT
#undef glDeleteFramebuffersEXT
#undef glGenFramebuffersEXT
#undef glCheckFramebufferStatusEXT
#undef glFramebufferTexture1DEXT
#undef glFramebufferTexture2DEXT
#undef glFramebufferTexture3DEXT
#undef glFramebufferRenderbufferEXT
#undef glGetFramebufferAttachmentParameterivEXT
#undef glGenerateMipmapEXT
#undef glFramebufferTextureLayerEXT
#undef glMapBufferRange
#undef glFlushMappedBufferRange
#undef glBindVertexArray
#undef glDeleteVertexArrays
#undef glGenVertexArrays
#undef glIsVertexArray
#undef glCopyBufferSubData
#undef glFenceSync
#undef glIsSync
#undef glDeleteSync
#undef glClientWaitSync
#undef glWaitSync
#undef glGetInteger64v
#undef glGetSynciv
#undef glProvokingVertexEXT
#undef glDrawElementsBaseVertex
#undef glDrawRangeElementsBaseVertex
#undef glMultiDrawElementsBaseVertex
#undef glProvokingVertex
#undef glRenderbufferStorageMultisampleEXT
#undef glColorMaskIndexedEXT
#undef glGetBooleanIndexedvEXT
#undef glGetIntegerIndexedvEXT
#undef glEnableIndexedEXT
#undef glDisableIndexedEXT
#undef glIsEnabledIndexedEXT
#undef glBeginConditionalRenderNV
#undef glEndConditionalRenderNV
#undef glObjectPurgeableAPPLE
#undef glObjectUnpurgeableAPPLE
#undef glGetObjectParameterivAPPLE
#undef glBeginTransformFeedback
#undef glEndTransformFeedback
#undef glBindBufferRange
#undef glBindBufferBase
#undef glTransformFeedbackVaryings
#undef glGetTransformFeedbackVarying
#undef glDrawArraysInstanced
#undef glDrawElementsInstanced
#undef glDrawArraysInstancedARB
#undef glDrawElementsInstancedARB
#undef glProgramParameteriARB
#undef glFramebufferTextureARB
#undef glFramebufferTextureFaceARB
#undef glBindTransformFeedback
#undef glDeleteTransformFeedbacks
#undef glGenTransformFeedbacks
#undef glIsTransformFeedback
#undef glPauseTransformFeedback
#undef glResumeTransformFeedback
#undef glDrawTransformFeedback
#undef glDrawArraysInstancedEXT
#undef glDrawElementsInstancedEXT
#undef glBeginTransformFeedbackEXT
#undef glEndTransformFeedbackEXT
#undef glBindBufferRangeEXT
#undef glBindBufferOffsetEXT
#undef glBindBufferBaseEXT
#undef glTransformFeedbackVaryingsEXT
#undef glGetTransformFeedbackVaryingEXT
#undef glEGLImageTargetTexture2DOES
#undef glEGLImageTargetRenderbufferStorageOES
#undef eglGetError
#undef eglGetDisplay
#undef eglInitialize
#undef eglTerminate
#undef eglQueryString
#undef eglGetConfigs
#undef eglChooseConfig
#undef eglGetConfigAttrib
#undef eglCreateWindowSurface
#undef eglCreatePbufferSurface
#undef eglCreatePixmapSurface
#undef eglDestroySurface
#undef eglQuerySurface
#undef eglBindAPI
#undef eglQueryAPI
#undef eglWaitClient
#undef eglReleaseThread
#undef eglCreatePbufferFromClientBuffer
#undef eglSurfaceAttrib
#undef eglBindTexImage
#undef eglReleaseTexImage
#undef eglSwapInterval
#undef eglCreateContext
#undef eglDestroyContext
#undef eglMakeCurrent
#undef eglGetCurrentContext
#undef eglGetCurrentSurface
#undef eglGetCurrentDisplay
#undef eglQueryContext
#undef eglWaitGL
#undef eglWaitNative
#undef eglSwapBuffers
#undef eglCopyBuffers
