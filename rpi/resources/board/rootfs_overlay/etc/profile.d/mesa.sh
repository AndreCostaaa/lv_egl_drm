# work-around for 'error: GLSL ES 3.00 is not supported. Supported versions are: 1.00 ES' as MESA is conservative
export MESA_GL_VERSION_OVERRIDE=3.0
export MESA_GLSL_VERSION_OVERRIDE=300
export MESA_GLES_VERSION_OVERRIDE=3.0
