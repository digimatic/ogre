#include <GLES3/gles3w.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <EGL/egl.h>

static HMODULE libgl;

static void open_libgl(void)
{
	libgl = LoadLibraryA("libGLESv2.dll");
}

static void close_libgl(void)
{
	FreeLibrary(libgl);
}

static void *get_proc(const char *proc)
{
	void *res;

	res = eglGetProcAddress(proc);
	if (!res)
		res = GetProcAddress(libgl, proc);
	return res;
}
#elif defined(__APPLE__) || defined(__APPLE_CC__)
#import <CoreFoundation/CoreFoundation.h>
#import <UIKit/UIDevice.h>

CFBundleRef bundle;
CFURLRef bundleURL;

static void open_libgl(void)
{
    CFStringRef frameworkPath = CFSTR("/System/Library/Frameworks/OpenGLES.framework");
    NSString *sysVersion = [UIDevice currentDevice].systemVersion;
    BOOL isSimulator = ([[UIDevice currentDevice].model rangeOfString:@"Simulator"].location != NSNotFound);
    if(isSimulator)
    {
        if([sysVersion isEqualToString:@"6.1"])
            frameworkPath = CFSTR("/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator6.1.sdk/System/Library/Frameworks/OpenGLES.framework");
        else if([sysVersion isEqualToString:@"6.0"])
            frameworkPath = CFSTR("/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator6.0.sdk/System/Library/Frameworks/OpenGLES.framework");
        else if([sysVersion isEqualToString:@"5.1"])
            frameworkPath = CFSTR("/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator5.1.sdk/System/Library/Frameworks/OpenGLES.framework");
        else if([sysVersion isEqualToString:@"5.0"])
            frameworkPath = CFSTR("/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator5.0.sdk/System/Library/Frameworks/OpenGLES.framework");
    }

	bundleURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault,
                                              frameworkPath,
                                              kCFURLPOSIXPathStyle, true);

	bundle = CFBundleCreate(kCFAllocatorDefault, bundleURL);
	assert(bundle != NULL);
}

static void close_libgl(void)
{
	CFRelease(bundle);
	CFRelease(bundleURL);
}

static void *get_proc(const char *proc)
{
	void *res;

	CFStringRef procname = CFStringCreateWithCString(kCFAllocatorDefault, proc,
                                                     kCFStringEncodingASCII);
	res = CFBundleGetFunctionPointerForName(bundle, procname);
	CFRelease(procname);
	return res;
}
#else
#include <dlfcn.h>
#include <EGL/egl.h>

static void *libgl;

static void open_libgl(void)
{
	libgl = dlopen("libGLESv2.so", RTLD_LAZY | RTLD_GLOBAL);
}

static void close_libgl(void)
{
	dlclose(libgl);
}

static void *get_proc(const char *proc)
{
	void *res;
    res = dlsym(libgl, proc);
	return res;
}
#endif

static struct {
	int major, minor;
} version;

static int parse_version(void)
{
	if (!glGetIntegerv)
		return -1;

	glGetIntegerv(GL_MAJOR_VERSION, &version.major);
	glGetIntegerv(GL_MINOR_VERSION, &version.minor);

	if (version.major < 3)
		return -1;
	return 0;
}

static void load_procs(void);

int gleswInit(void)
{
	open_libgl();
	load_procs();
	close_libgl();
	return parse_version();
}

int gleswIsSupported(int major, int minor)
{
	if (major < 3)
		return 0;
	if (version.major == major)
		return version.minor >= minor;
	return version.major >= major;
}

void *gleswGetProcAddress(const char *proc)
{
	return get_proc(proc);
}

PFNGLACTIVETEXTUREPROC gleswActiveTexture;
PFNGLATTACHSHADERPROC gleswAttachShader;
PFNGLBINDATTRIBLOCATIONPROC gleswBindAttribLocation;
PFNGLBINDBUFFERPROC gleswBindBuffer;
PFNGLBINDFRAMEBUFFERPROC gleswBindFramebuffer;
PFNGLBINDRENDERBUFFERPROC gleswBindRenderbuffer;
PFNGLBINDTEXTUREPROC gleswBindTexture;
PFNGLBLENDCOLORPROC gleswBlendColor;
PFNGLBLENDEQUATIONPROC gleswBlendEquation;
PFNGLBLENDEQUATIONSEPARATEPROC gleswBlendEquationSeparate;
PFNGLBLENDFUNCPROC gleswBlendFunc;
PFNGLBLENDFUNCSEPARATEPROC gleswBlendFuncSeparate;
PFNGLBUFFERDATAPROC gleswBufferData;
PFNGLBUFFERSUBDATAPROC gleswBufferSubData;
PFNGLCHECKFRAMEBUFFERSTATUSPROC gleswCheckFramebufferStatus;
PFNGLCLEARPROC gleswClear;
PFNGLCLEARCOLORPROC gleswClearColor;
PFNGLCLEARDEPTHFPROC gleswClearDepthf;
PFNGLCLEARSTENCILPROC gleswClearStencil;
PFNGLCOLORMASKPROC gleswColorMask;
PFNGLCOMPILESHADERPROC gleswCompileShader;
PFNGLCOMPRESSEDTEXIMAGE2DPROC gleswCompressedTexImage2D;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC gleswCompressedTexSubImage2D;
PFNGLCOPYTEXIMAGE2DPROC gleswCopyTexImage2D;
PFNGLCOPYTEXSUBIMAGE2DPROC gleswCopyTexSubImage2D;
PFNGLCREATEPROGRAMPROC gleswCreateProgram;
PFNGLCREATESHADERPROC gleswCreateShader;
PFNGLCULLFACEPROC gleswCullFace;
PFNGLDELETEBUFFERSPROC gleswDeleteBuffers;
PFNGLDELETEFRAMEBUFFERSPROC gleswDeleteFramebuffers;
PFNGLDELETEPROGRAMPROC gleswDeleteProgram;
PFNGLDELETERENDERBUFFERSPROC gleswDeleteRenderbuffers;
PFNGLDELETESHADERPROC gleswDeleteShader;
PFNGLDELETETEXTURESPROC gleswDeleteTextures;
PFNGLDEPTHFUNCPROC gleswDepthFunc;
PFNGLDEPTHMASKPROC gleswDepthMask;
PFNGLDEPTHRANGEFPROC gleswDepthRangef;
PFNGLDETACHSHADERPROC gleswDetachShader;
PFNGLDISABLEPROC gleswDisable;
PFNGLDISABLEVERTEXATTRIBARRAYPROC gleswDisableVertexAttribArray;
PFNGLDRAWARRAYSPROC gleswDrawArrays;
PFNGLDRAWELEMENTSPROC gleswDrawElements;
PFNGLENABLEPROC gleswEnable;
PFNGLENABLEVERTEXATTRIBARRAYPROC gleswEnableVertexAttribArray;
PFNGLFINISHPROC gleswFinish;
PFNGLFLUSHPROC gleswFlush;
PFNGLFRAMEBUFFERRENDERBUFFERPROC gleswFramebufferRenderbuffer;
PFNGLFRAMEBUFFERTEXTURE2DPROC gleswFramebufferTexture2D;
PFNGLFRONTFACEPROC gleswFrontFace;
PFNGLGENBUFFERSPROC gleswGenBuffers;
PFNGLGENERATEMIPMAPPROC gleswGenerateMipmap;
PFNGLGENFRAMEBUFFERSPROC gleswGenFramebuffers;
PFNGLGENRENDERBUFFERSPROC gleswGenRenderbuffers;
PFNGLGENTEXTURESPROC gleswGenTextures;
PFNGLGETACTIVEATTRIBPROC gleswGetActiveAttrib;
PFNGLGETACTIVEUNIFORMPROC gleswGetActiveUniform;
PFNGLGETATTACHEDSHADERSPROC gleswGetAttachedShaders;
PFNGLGETATTRIBLOCATIONPROC gleswGetAttribLocation;
PFNGLGETBOOLEANVPROC gleswGetBooleanv;
PFNGLGETBUFFERPARAMETERIVPROC gleswGetBufferParameteriv;
PFNGLGETERRORPROC gleswGetError;
PFNGLGETFLOATVPROC gleswGetFloatv;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC gleswGetFramebufferAttachmentParameteriv;
PFNGLGETINTEGERVPROC gleswGetIntegerv;
PFNGLGETPROGRAMIVPROC gleswGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC gleswGetProgramInfoLog;
PFNGLGETRENDERBUFFERPARAMETERIVPROC gleswGetRenderbufferParameteriv;
PFNGLGETSHADERIVPROC gleswGetShaderiv;
PFNGLGETSHADERINFOLOGPROC gleswGetShaderInfoLog;
PFNGLGETSHADERPRECISIONFORMATPROC gleswGetShaderPrecisionFormat;
PFNGLGETSHADERSOURCEPROC gleswGetShaderSource;
PFNGLGETSTRINGPROC gleswGetString;
PFNGLGETTEXPARAMETERFVPROC gleswGetTexParameterfv;
PFNGLGETTEXPARAMETERIVPROC gleswGetTexParameteriv;
PFNGLGETUNIFORMFVPROC gleswGetUniformfv;
PFNGLGETUNIFORMIVPROC gleswGetUniformiv;
PFNGLGETUNIFORMLOCATIONPROC gleswGetUniformLocation;
PFNGLGETVERTEXATTRIBFVPROC gleswGetVertexAttribfv;
PFNGLGETVERTEXATTRIBIVPROC gleswGetVertexAttribiv;
PFNGLGETVERTEXATTRIBPOINTERVPROC gleswGetVertexAttribPointerv;
PFNGLHINTPROC gleswHint;
PFNGLISBUFFERPROC gleswIsBuffer;
PFNGLISENABLEDPROC gleswIsEnabled;
PFNGLISFRAMEBUFFERPROC gleswIsFramebuffer;
PFNGLISPROGRAMPROC gleswIsProgram;
PFNGLISRENDERBUFFERPROC gleswIsRenderbuffer;
PFNGLISSHADERPROC gleswIsShader;
PFNGLISTEXTUREPROC gleswIsTexture;
PFNGLLINEWIDTHPROC gleswLineWidth;
PFNGLLINKPROGRAMPROC gleswLinkProgram;
PFNGLPIXELSTOREIPROC gleswPixelStorei;
PFNGLPOLYGONOFFSETPROC gleswPolygonOffset;
PFNGLREADPIXELSPROC gleswReadPixels;
PFNGLRELEASESHADERCOMPILERPROC gleswReleaseShaderCompiler;
PFNGLRENDERBUFFERSTORAGEPROC gleswRenderbufferStorage;
PFNGLSAMPLECOVERAGEPROC gleswSampleCoverage;
PFNGLSCISSORPROC gleswScissor;
PFNGLSHADERBINARYPROC gleswShaderBinary;
PFNGLSHADERSOURCEPROC gleswShaderSource;
PFNGLSTENCILFUNCPROC gleswStencilFunc;
PFNGLSTENCILFUNCSEPARATEPROC gleswStencilFuncSeparate;
PFNGLSTENCILMASKPROC gleswStencilMask;
PFNGLSTENCILMASKSEPARATEPROC gleswStencilMaskSeparate;
PFNGLSTENCILOPPROC gleswStencilOp;
PFNGLSTENCILOPSEPARATEPROC gleswStencilOpSeparate;
PFNGLTEXIMAGE2DPROC gleswTexImage2D;
PFNGLTEXPARAMETERFPROC gleswTexParameterf;
PFNGLTEXPARAMETERFVPROC gleswTexParameterfv;
PFNGLTEXPARAMETERIPROC gleswTexParameteri;
PFNGLTEXPARAMETERIVPROC gleswTexParameteriv;
PFNGLTEXSUBIMAGE2DPROC gleswTexSubImage2D;
PFNGLUNIFORM1FPROC gleswUniform1f;
PFNGLUNIFORM1FVPROC gleswUniform1fv;
PFNGLUNIFORM1IPROC gleswUniform1i;
PFNGLUNIFORM1IVPROC gleswUniform1iv;
PFNGLUNIFORM2FPROC gleswUniform2f;
PFNGLUNIFORM2FVPROC gleswUniform2fv;
PFNGLUNIFORM2IPROC gleswUniform2i;
PFNGLUNIFORM2IVPROC gleswUniform2iv;
PFNGLUNIFORM3FPROC gleswUniform3f;
PFNGLUNIFORM3FVPROC gleswUniform3fv;
PFNGLUNIFORM3IPROC gleswUniform3i;
PFNGLUNIFORM3IVPROC gleswUniform3iv;
PFNGLUNIFORM4FPROC gleswUniform4f;
PFNGLUNIFORM4FVPROC gleswUniform4fv;
PFNGLUNIFORM4IPROC gleswUniform4i;
PFNGLUNIFORM4IVPROC gleswUniform4iv;
PFNGLUNIFORMMATRIX2FVPROC gleswUniformMatrix2fv;
PFNGLUNIFORMMATRIX3FVPROC gleswUniformMatrix3fv;
PFNGLUNIFORMMATRIX4FVPROC gleswUniformMatrix4fv;
PFNGLUSEPROGRAMPROC gleswUseProgram;
PFNGLVALIDATEPROGRAMPROC gleswValidateProgram;
PFNGLVERTEXATTRIB1FPROC gleswVertexAttrib1f;
PFNGLVERTEXATTRIB1FVPROC gleswVertexAttrib1fv;
PFNGLVERTEXATTRIB2FPROC gleswVertexAttrib2f;
PFNGLVERTEXATTRIB2FVPROC gleswVertexAttrib2fv;
PFNGLVERTEXATTRIB3FPROC gleswVertexAttrib3f;
PFNGLVERTEXATTRIB3FVPROC gleswVertexAttrib3fv;
PFNGLVERTEXATTRIB4FPROC gleswVertexAttrib4f;
PFNGLVERTEXATTRIB4FVPROC gleswVertexAttrib4fv;
PFNGLVERTEXATTRIBPOINTERPROC gleswVertexAttribPointer;
PFNGLVIEWPORTPROC gleswViewport;
PFNGLREADBUFFERPROC gleswReadBuffer;
PFNGLDRAWRANGEELEMENTSPROC gleswDrawRangeElements;
PFNGLTEXIMAGE3DPROC gleswTexImage3D;
PFNGLTEXSUBIMAGE3DPROC gleswTexSubImage3D;
PFNGLCOPYTEXSUBIMAGE3DPROC gleswCopyTexSubImage3D;
PFNGLCOMPRESSEDTEXIMAGE3DPROC gleswCompressedTexImage3D;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC gleswCompressedTexSubImage3D;
PFNGLGENQUERIESPROC gleswGenQueries;
PFNGLDELETEQUERIESPROC gleswDeleteQueries;
PFNGLISQUERYPROC gleswIsQuery;
PFNGLBEGINQUERYPROC gleswBeginQuery;
PFNGLENDQUERYPROC gleswEndQuery;
PFNGLGETQUERYIVPROC gleswGetQueryiv;
PFNGLGETQUERYOBJECTUIVPROC gleswGetQueryObjectuiv;
PFNGLUNMAPBUFFERPROC gleswUnmapBuffer;
PFNGLGETBUFFERPOINTERVPROC gleswGetBufferPointerv;
PFNGLDRAWBUFFERSPROC gleswDrawBuffers;
PFNGLUNIFORMMATRIX2X3FVPROC gleswUniformMatrix2x3fv;
PFNGLUNIFORMMATRIX3X2FVPROC gleswUniformMatrix3x2fv;
PFNGLUNIFORMMATRIX2X4FVPROC gleswUniformMatrix2x4fv;
PFNGLUNIFORMMATRIX4X2FVPROC gleswUniformMatrix4x2fv;
PFNGLUNIFORMMATRIX3X4FVPROC gleswUniformMatrix3x4fv;
PFNGLUNIFORMMATRIX4X3FVPROC gleswUniformMatrix4x3fv;
PFNGLBLITFRAMEBUFFERPROC gleswBlitFramebuffer;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC gleswRenderbufferStorageMultisample;
PFNGLFRAMEBUFFERTEXTURELAYERPROC gleswFramebufferTextureLayer;
PFNGLMAPBUFFERRANGEPROC gleswMapBufferRange;
PFNGLFLUSHMAPPEDBUFFERRANGEPROC gleswFlushMappedBufferRange;
PFNGLBINDVERTEXARRAYPROC gleswBindVertexArray;
PFNGLDELETEVERTEXARRAYSPROC gleswDeleteVertexArrays;
PFNGLGENVERTEXARRAYSPROC gleswGenVertexArrays;
PFNGLISVERTEXARRAYPROC gleswIsVertexArray;
PFNGLGETINTEGERI_VPROC gleswGetIntegeri_v;
PFNGLBEGINTRANSFORMFEEDBACKPROC gleswBeginTransformFeedback;
PFNGLENDTRANSFORMFEEDBACKPROC gleswEndTransformFeedback;
PFNGLBINDBUFFERRANGEPROC gleswBindBufferRange;
PFNGLBINDBUFFERBASEPROC gleswBindBufferBase;
PFNGLTRANSFORMFEEDBACKVARYINGSPROC gleswTransformFeedbackVaryings;
PFNGLGETTRANSFORMFEEDBACKVARYINGPROC gleswGetTransformFeedbackVarying;
PFNGLVERTEXATTRIBIPOINTERPROC gleswVertexAttribIPointer;
PFNGLGETVERTEXATTRIBIIVPROC gleswGetVertexAttribIiv;
PFNGLGETVERTEXATTRIBIUIVPROC gleswGetVertexAttribIuiv;
PFNGLVERTEXATTRIBI4IPROC gleswVertexAttribI4i;
PFNGLVERTEXATTRIBI4UIPROC gleswVertexAttribI4ui;
PFNGLVERTEXATTRIBI4IVPROC gleswVertexAttribI4iv;
PFNGLVERTEXATTRIBI4UIVPROC gleswVertexAttribI4uiv;
PFNGLGETUNIFORMUIVPROC gleswGetUniformuiv;
PFNGLGETFRAGDATALOCATIONPROC gleswGetFragDataLocation;
PFNGLUNIFORM1UIPROC gleswUniform1ui;
PFNGLUNIFORM2UIPROC gleswUniform2ui;
PFNGLUNIFORM3UIPROC gleswUniform3ui;
PFNGLUNIFORM4UIPROC gleswUniform4ui;
PFNGLUNIFORM1UIVPROC gleswUniform1uiv;
PFNGLUNIFORM2UIVPROC gleswUniform2uiv;
PFNGLUNIFORM3UIVPROC gleswUniform3uiv;
PFNGLUNIFORM4UIVPROC gleswUniform4uiv;
PFNGLCLEARBUFFERIVPROC gleswClearBufferiv;
PFNGLCLEARBUFFERUIVPROC gleswClearBufferuiv;
PFNGLCLEARBUFFERFVPROC gleswClearBufferfv;
PFNGLCLEARBUFFERFIPROC gleswClearBufferfi;
PFNGLGETSTRINGIPROC gleswGetStringi;
PFNGLCOPYBUFFERSUBDATAPROC gleswCopyBufferSubData;
PFNGLGETUNIFORMINDICESPROC gleswGetUniformIndices;
PFNGLGETACTIVEUNIFORMSIVPROC gleswGetActiveUniformsiv;
PFNGLGETUNIFORMBLOCKINDEXPROC gleswGetUniformBlockIndex;
PFNGLGETACTIVEUNIFORMBLOCKIVPROC gleswGetActiveUniformBlockiv;
PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC gleswGetActiveUniformBlockName;
PFNGLUNIFORMBLOCKBINDINGPROC gleswUniformBlockBinding;
PFNGLDRAWARRAYSINSTANCEDPROC gleswDrawArraysInstanced;
PFNGLDRAWELEMENTSINSTANCEDPROC gleswDrawElementsInstanced;
PFNGLFENCESYNCPROC gleswFenceSync;
PFNGLISSYNCPROC gleswIsSync;
PFNGLDELETESYNCPROC gleswDeleteSync;
PFNGLCLIENTWAITSYNCPROC gleswClientWaitSync;
PFNGLWAITSYNCPROC gleswWaitSync;
PFNGLGETINTEGER64VPROC gleswGetInteger64v;
PFNGLGETSYNCIVPROC gleswGetSynciv;
PFNGLGETINTEGER64I_VPROC gleswGetInteger64i_v;
PFNGLGETBUFFERPARAMETERI64VPROC gleswGetBufferParameteri64v;
PFNGLGENSAMPLERSPROC gleswGenSamplers;
PFNGLDELETESAMPLERSPROC gleswDeleteSamplers;
PFNGLISSAMPLERPROC gleswIsSampler;
PFNGLBINDSAMPLERPROC gleswBindSampler;
PFNGLSAMPLERPARAMETERIPROC gleswSamplerParameteri;
PFNGLSAMPLERPARAMETERIVPROC gleswSamplerParameteriv;
PFNGLSAMPLERPARAMETERFPROC gleswSamplerParameterf;
PFNGLSAMPLERPARAMETERFVPROC gleswSamplerParameterfv;
PFNGLGETSAMPLERPARAMETERIVPROC gleswGetSamplerParameteriv;
PFNGLGETSAMPLERPARAMETERFVPROC gleswGetSamplerParameterfv;
PFNGLVERTEXATTRIBDIVISORPROC gleswVertexAttribDivisor;
PFNGLBINDTRANSFORMFEEDBACKPROC gleswBindTransformFeedback;
PFNGLDELETETRANSFORMFEEDBACKSPROC gleswDeleteTransformFeedbacks;
PFNGLGENTRANSFORMFEEDBACKSPROC gleswGenTransformFeedbacks;
PFNGLISTRANSFORMFEEDBACKPROC gleswIsTransformFeedback;
PFNGLPAUSETRANSFORMFEEDBACKPROC gleswPauseTransformFeedback;
PFNGLRESUMETRANSFORMFEEDBACKPROC gleswResumeTransformFeedback;
PFNGLGETPROGRAMBINARYPROC gleswGetProgramBinary;
PFNGLPROGRAMBINARYPROC gleswProgramBinary;
PFNGLPROGRAMPARAMETERIPROC gleswProgramParameteri;
PFNGLINVALIDATEFRAMEBUFFERPROC gleswInvalidateFramebuffer;
PFNGLINVALIDATESUBFRAMEBUFFERPROC gleswInvalidateSubFramebuffer;
PFNGLTEXSTORAGE2DPROC gleswTexStorage2D;
PFNGLTEXSTORAGE3DPROC gleswTexStorage3D;
PFNGLGETINTERNALFORMATIVPROC gleswGetInternalformativ;

static void load_procs(void)
{
	gleswActiveTexture = (PFNGLACTIVETEXTUREPROC) get_proc("glActiveTexture");
	gleswAttachShader = (PFNGLATTACHSHADERPROC) get_proc("glAttachShader");
	gleswBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC) get_proc("glBindAttribLocation");
	gleswBindBuffer = (PFNGLBINDBUFFERPROC) get_proc("glBindBuffer");
	gleswBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) get_proc("glBindFramebuffer");
	gleswBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC) get_proc("glBindRenderbuffer");
	gleswBindTexture = (PFNGLBINDTEXTUREPROC) get_proc("glBindTexture");
	gleswBlendColor = (PFNGLBLENDCOLORPROC) get_proc("glBlendColor");
	gleswBlendEquation = (PFNGLBLENDEQUATIONPROC) get_proc("glBlendEquation");
	gleswBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC) get_proc("glBlendEquationSeparate");
	gleswBlendFunc = (PFNGLBLENDFUNCPROC) get_proc("glBlendFunc");
	gleswBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC) get_proc("glBlendFuncSeparate");
	gleswBufferData = (PFNGLBUFFERDATAPROC) get_proc("glBufferData");
	gleswBufferSubData = (PFNGLBUFFERSUBDATAPROC) get_proc("glBufferSubData");
	gleswCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) get_proc("glCheckFramebufferStatus");
	gleswClear = (PFNGLCLEARPROC) get_proc("glClear");
	gleswClearColor = (PFNGLCLEARCOLORPROC) get_proc("glClearColor");
	gleswClearDepthf = (PFNGLCLEARDEPTHFPROC) get_proc("glClearDepthf");
	gleswClearStencil = (PFNGLCLEARSTENCILPROC) get_proc("glClearStencil");
	gleswColorMask = (PFNGLCOLORMASKPROC) get_proc("glColorMask");
	gleswCompileShader = (PFNGLCOMPILESHADERPROC) get_proc("glCompileShader");
	gleswCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC) get_proc("glCompressedTexImage2D");
	gleswCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) get_proc("glCompressedTexSubImage2D");
	gleswCopyTexImage2D = (PFNGLCOPYTEXIMAGE2DPROC) get_proc("glCopyTexImage2D");
	gleswCopyTexSubImage2D = (PFNGLCOPYTEXSUBIMAGE2DPROC) get_proc("glCopyTexSubImage2D");
	gleswCreateProgram = (PFNGLCREATEPROGRAMPROC) get_proc("glCreateProgram");
	gleswCreateShader = (PFNGLCREATESHADERPROC) get_proc("glCreateShader");
	gleswCullFace = (PFNGLCULLFACEPROC) get_proc("glCullFace");
	gleswDeleteBuffers = (PFNGLDELETEBUFFERSPROC) get_proc("glDeleteBuffers");
	gleswDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) get_proc("glDeleteFramebuffers");
	gleswDeleteProgram = (PFNGLDELETEPROGRAMPROC) get_proc("glDeleteProgram");
	gleswDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC) get_proc("glDeleteRenderbuffers");
	gleswDeleteShader = (PFNGLDELETESHADERPROC) get_proc("glDeleteShader");
	gleswDeleteTextures = (PFNGLDELETETEXTURESPROC) get_proc("glDeleteTextures");
	gleswDepthFunc = (PFNGLDEPTHFUNCPROC) get_proc("glDepthFunc");
	gleswDepthMask = (PFNGLDEPTHMASKPROC) get_proc("glDepthMask");
	gleswDepthRangef = (PFNGLDEPTHRANGEFPROC) get_proc("glDepthRangef");
	gleswDetachShader = (PFNGLDETACHSHADERPROC) get_proc("glDetachShader");
	gleswDisable = (PFNGLDISABLEPROC) get_proc("glDisable");
	gleswDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) get_proc("glDisableVertexAttribArray");
	gleswDrawArrays = (PFNGLDRAWARRAYSPROC) get_proc("glDrawArrays");
	gleswDrawElements = (PFNGLDRAWELEMENTSPROC) get_proc("glDrawElements");
	gleswEnable = (PFNGLENABLEPROC) get_proc("glEnable");
	gleswEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) get_proc("glEnableVertexAttribArray");
	gleswFinish = (PFNGLFINISHPROC) get_proc("glFinish");
	gleswFlush = (PFNGLFLUSHPROC) get_proc("glFlush");
	gleswFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) get_proc("glFramebufferRenderbuffer");
	gleswFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) get_proc("glFramebufferTexture2D");
	gleswFrontFace = (PFNGLFRONTFACEPROC) get_proc("glFrontFace");
	gleswGenBuffers = (PFNGLGENBUFFERSPROC) get_proc("glGenBuffers");
	gleswGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) get_proc("glGenerateMipmap");
	gleswGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) get_proc("glGenFramebuffers");
	gleswGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC) get_proc("glGenRenderbuffers");
	gleswGenTextures = (PFNGLGENTEXTURESPROC) get_proc("glGenTextures");
	gleswGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC) get_proc("glGetActiveAttrib");
	gleswGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC) get_proc("glGetActiveUniform");
	gleswGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC) get_proc("glGetAttachedShaders");
	gleswGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) get_proc("glGetAttribLocation");
	gleswGetBooleanv = (PFNGLGETBOOLEANVPROC) get_proc("glGetBooleanv");
	gleswGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC) get_proc("glGetBufferParameteriv");
	gleswGetError = (PFNGLGETERRORPROC) get_proc("glGetError");
	gleswGetFloatv = (PFNGLGETFLOATVPROC) get_proc("glGetFloatv");
	gleswGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) get_proc("glGetFramebufferAttachmentParameteriv");
	gleswGetIntegerv = (PFNGLGETINTEGERVPROC) get_proc("glGetIntegerv");
	gleswGetProgramiv = (PFNGLGETPROGRAMIVPROC) get_proc("glGetProgramiv");
	gleswGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) get_proc("glGetProgramInfoLog");
	gleswGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) get_proc("glGetRenderbufferParameteriv");
	gleswGetShaderiv = (PFNGLGETSHADERIVPROC) get_proc("glGetShaderiv");
	gleswGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) get_proc("glGetShaderInfoLog");
	gleswGetShaderPrecisionFormat = (PFNGLGETSHADERPRECISIONFORMATPROC) get_proc("glGetShaderPrecisionFormat");
	gleswGetShaderSource = (PFNGLGETSHADERSOURCEPROC) get_proc("glGetShaderSource");
	gleswGetString = (PFNGLGETSTRINGPROC) get_proc("glGetString");
	gleswGetTexParameterfv = (PFNGLGETTEXPARAMETERFVPROC) get_proc("glGetTexParameterfv");
	gleswGetTexParameteriv = (PFNGLGETTEXPARAMETERIVPROC) get_proc("glGetTexParameteriv");
	gleswGetUniformfv = (PFNGLGETUNIFORMFVPROC) get_proc("glGetUniformfv");
	gleswGetUniformiv = (PFNGLGETUNIFORMIVPROC) get_proc("glGetUniformiv");
	gleswGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) get_proc("glGetUniformLocation");
	gleswGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC) get_proc("glGetVertexAttribfv");
	gleswGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC) get_proc("glGetVertexAttribiv");
	gleswGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC) get_proc("glGetVertexAttribPointerv");
	gleswHint = (PFNGLHINTPROC) get_proc("glHint");
	gleswIsBuffer = (PFNGLISBUFFERPROC) get_proc("glIsBuffer");
	gleswIsEnabled = (PFNGLISENABLEDPROC) get_proc("glIsEnabled");
	gleswIsFramebuffer = (PFNGLISFRAMEBUFFERPROC) get_proc("glIsFramebuffer");
	gleswIsProgram = (PFNGLISPROGRAMPROC) get_proc("glIsProgram");
	gleswIsRenderbuffer = (PFNGLISRENDERBUFFERPROC) get_proc("glIsRenderbuffer");
	gleswIsShader = (PFNGLISSHADERPROC) get_proc("glIsShader");
	gleswIsTexture = (PFNGLISTEXTUREPROC) get_proc("glIsTexture");
	gleswLineWidth = (PFNGLLINEWIDTHPROC) get_proc("glLineWidth");
	gleswLinkProgram = (PFNGLLINKPROGRAMPROC) get_proc("glLinkProgram");
	gleswPixelStorei = (PFNGLPIXELSTOREIPROC) get_proc("glPixelStorei");
	gleswPolygonOffset = (PFNGLPOLYGONOFFSETPROC) get_proc("glPolygonOffset");
	gleswReadPixels = (PFNGLREADPIXELSPROC) get_proc("glReadPixels");
	gleswReleaseShaderCompiler = (PFNGLRELEASESHADERCOMPILERPROC) get_proc("glReleaseShaderCompiler");
	gleswRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC) get_proc("glRenderbufferStorage");
	gleswSampleCoverage = (PFNGLSAMPLECOVERAGEPROC) get_proc("glSampleCoverage");
	gleswScissor = (PFNGLSCISSORPROC) get_proc("glScissor");
	gleswShaderBinary = (PFNGLSHADERBINARYPROC) get_proc("glShaderBinary");
	gleswShaderSource = (PFNGLSHADERSOURCEPROC) get_proc("glShaderSource");
	gleswStencilFunc = (PFNGLSTENCILFUNCPROC) get_proc("glStencilFunc");
	gleswStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC) get_proc("glStencilFuncSeparate");
	gleswStencilMask = (PFNGLSTENCILMASKPROC) get_proc("glStencilMask");
	gleswStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC) get_proc("glStencilMaskSeparate");
	gleswStencilOp = (PFNGLSTENCILOPPROC) get_proc("glStencilOp");
	gleswStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC) get_proc("glStencilOpSeparate");
	gleswTexImage2D = (PFNGLTEXIMAGE2DPROC) get_proc("glTexImage2D");
	gleswTexParameterf = (PFNGLTEXPARAMETERFPROC) get_proc("glTexParameterf");
	gleswTexParameterfv = (PFNGLTEXPARAMETERFVPROC) get_proc("glTexParameterfv");
	gleswTexParameteri = (PFNGLTEXPARAMETERIPROC) get_proc("glTexParameteri");
	gleswTexParameteriv = (PFNGLTEXPARAMETERIVPROC) get_proc("glTexParameteriv");
	gleswTexSubImage2D = (PFNGLTEXSUBIMAGE2DPROC) get_proc("glTexSubImage2D");
	gleswUniform1f = (PFNGLUNIFORM1FPROC) get_proc("glUniform1f");
	gleswUniform1fv = (PFNGLUNIFORM1FVPROC) get_proc("glUniform1fv");
	gleswUniform1i = (PFNGLUNIFORM1IPROC) get_proc("glUniform1i");
	gleswUniform1iv = (PFNGLUNIFORM1IVPROC) get_proc("glUniform1iv");
	gleswUniform2f = (PFNGLUNIFORM2FPROC) get_proc("glUniform2f");
	gleswUniform2fv = (PFNGLUNIFORM2FVPROC) get_proc("glUniform2fv");
	gleswUniform2i = (PFNGLUNIFORM2IPROC) get_proc("glUniform2i");
	gleswUniform2iv = (PFNGLUNIFORM2IVPROC) get_proc("glUniform2iv");
	gleswUniform3f = (PFNGLUNIFORM3FPROC) get_proc("glUniform3f");
	gleswUniform3fv = (PFNGLUNIFORM3FVPROC) get_proc("glUniform3fv");
	gleswUniform3i = (PFNGLUNIFORM3IPROC) get_proc("glUniform3i");
	gleswUniform3iv = (PFNGLUNIFORM3IVPROC) get_proc("glUniform3iv");
	gleswUniform4f = (PFNGLUNIFORM4FPROC) get_proc("glUniform4f");
	gleswUniform4fv = (PFNGLUNIFORM4FVPROC) get_proc("glUniform4fv");
	gleswUniform4i = (PFNGLUNIFORM4IPROC) get_proc("glUniform4i");
	gleswUniform4iv = (PFNGLUNIFORM4IVPROC) get_proc("glUniform4iv");
	gleswUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC) get_proc("glUniformMatrix2fv");
	gleswUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC) get_proc("glUniformMatrix3fv");
	gleswUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC) get_proc("glUniformMatrix4fv");
	gleswUseProgram = (PFNGLUSEPROGRAMPROC) get_proc("glUseProgram");
	gleswValidateProgram = (PFNGLVALIDATEPROGRAMPROC) get_proc("glValidateProgram");
	gleswVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC) get_proc("glVertexAttrib1f");
	gleswVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC) get_proc("glVertexAttrib1fv");
	gleswVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC) get_proc("glVertexAttrib2f");
	gleswVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC) get_proc("glVertexAttrib2fv");
	gleswVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC) get_proc("glVertexAttrib3f");
	gleswVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC) get_proc("glVertexAttrib3fv");
	gleswVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC) get_proc("glVertexAttrib4f");
	gleswVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC) get_proc("glVertexAttrib4fv");
	gleswVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) get_proc("glVertexAttribPointer");
	gleswViewport = (PFNGLVIEWPORTPROC) get_proc("glViewport");
	gleswReadBuffer = (PFNGLREADBUFFERPROC) get_proc("glReadBuffer");
	gleswDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC) get_proc("glDrawRangeElements");
	gleswTexImage3D = (PFNGLTEXIMAGE3DPROC) get_proc("glTexImage3D");
	gleswTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC) get_proc("glTexSubImage3D");
	gleswCopyTexSubImage3D = (PFNGLCOPYTEXSUBIMAGE3DPROC) get_proc("glCopyTexSubImage3D");
	gleswCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC) get_proc("glCompressedTexImage3D");
	gleswCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) get_proc("glCompressedTexSubImage3D");
	gleswGenQueries = (PFNGLGENQUERIESPROC) get_proc("glGenQueries");
	gleswDeleteQueries = (PFNGLDELETEQUERIESPROC) get_proc("glDeleteQueries");
	gleswIsQuery = (PFNGLISQUERYPROC) get_proc("glIsQuery");
	gleswBeginQuery = (PFNGLBEGINQUERYPROC) get_proc("glBeginQuery");
	gleswEndQuery = (PFNGLENDQUERYPROC) get_proc("glEndQuery");
	gleswGetQueryiv = (PFNGLGETQUERYIVPROC) get_proc("glGetQueryiv");
	gleswGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC) get_proc("glGetQueryObjectuiv");
	gleswUnmapBuffer = (PFNGLUNMAPBUFFERPROC) get_proc("glUnmapBuffer");
	gleswGetBufferPointerv = (PFNGLGETBUFFERPOINTERVPROC) get_proc("glGetBufferPointerv");
	gleswDrawBuffers = (PFNGLDRAWBUFFERSPROC) get_proc("glDrawBuffers");
	gleswUniformMatrix2x3fv = (PFNGLUNIFORMMATRIX2X3FVPROC) get_proc("glUniformMatrix2x3fv");
	gleswUniformMatrix3x2fv = (PFNGLUNIFORMMATRIX3X2FVPROC) get_proc("glUniformMatrix3x2fv");
	gleswUniformMatrix2x4fv = (PFNGLUNIFORMMATRIX2X4FVPROC) get_proc("glUniformMatrix2x4fv");
	gleswUniformMatrix4x2fv = (PFNGLUNIFORMMATRIX4X2FVPROC) get_proc("glUniformMatrix4x2fv");
	gleswUniformMatrix3x4fv = (PFNGLUNIFORMMATRIX3X4FVPROC) get_proc("glUniformMatrix3x4fv");
	gleswUniformMatrix4x3fv = (PFNGLUNIFORMMATRIX4X3FVPROC) get_proc("glUniformMatrix4x3fv");
	gleswBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC) get_proc("glBlitFramebuffer");
	gleswRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC) get_proc("glRenderbufferStorageMultisample");
	gleswFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC) get_proc("glFramebufferTextureLayer");
	gleswMapBufferRange = (PFNGLMAPBUFFERRANGEPROC) get_proc("glMapBufferRange");
	gleswFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC) get_proc("glFlushMappedBufferRange");
	gleswBindVertexArray = (PFNGLBINDVERTEXARRAYPROC) get_proc("glBindVertexArray");
	gleswDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC) get_proc("glDeleteVertexArrays");
	gleswGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC) get_proc("glGenVertexArrays");
	gleswIsVertexArray = (PFNGLISVERTEXARRAYPROC) get_proc("glIsVertexArray");
	gleswGetIntegeri_v = (PFNGLGETINTEGERI_VPROC) get_proc("glGetIntegeri_v");
	gleswBeginTransformFeedback = (PFNGLBEGINTRANSFORMFEEDBACKPROC) get_proc("glBeginTransformFeedback");
	gleswEndTransformFeedback = (PFNGLENDTRANSFORMFEEDBACKPROC) get_proc("glEndTransformFeedback");
	gleswBindBufferRange = (PFNGLBINDBUFFERRANGEPROC) get_proc("glBindBufferRange");
	gleswBindBufferBase = (PFNGLBINDBUFFERBASEPROC) get_proc("glBindBufferBase");
	gleswTransformFeedbackVaryings = (PFNGLTRANSFORMFEEDBACKVARYINGSPROC) get_proc("glTransformFeedbackVaryings");
	gleswGetTransformFeedbackVarying = (PFNGLGETTRANSFORMFEEDBACKVARYINGPROC) get_proc("glGetTransformFeedbackVarying");
	gleswVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC) get_proc("glVertexAttribIPointer");
	gleswGetVertexAttribIiv = (PFNGLGETVERTEXATTRIBIIVPROC) get_proc("glGetVertexAttribIiv");
	gleswGetVertexAttribIuiv = (PFNGLGETVERTEXATTRIBIUIVPROC) get_proc("glGetVertexAttribIuiv");
	gleswVertexAttribI4i = (PFNGLVERTEXATTRIBI4IPROC) get_proc("glVertexAttribI4i");
	gleswVertexAttribI4ui = (PFNGLVERTEXATTRIBI4UIPROC) get_proc("glVertexAttribI4ui");
	gleswVertexAttribI4iv = (PFNGLVERTEXATTRIBI4IVPROC) get_proc("glVertexAttribI4iv");
	gleswVertexAttribI4uiv = (PFNGLVERTEXATTRIBI4UIVPROC) get_proc("glVertexAttribI4uiv");
	gleswGetUniformuiv = (PFNGLGETUNIFORMUIVPROC) get_proc("glGetUniformuiv");
	gleswGetFragDataLocation = (PFNGLGETFRAGDATALOCATIONPROC) get_proc("glGetFragDataLocation");
	gleswUniform1ui = (PFNGLUNIFORM1UIPROC) get_proc("glUniform1ui");
	gleswUniform2ui = (PFNGLUNIFORM2UIPROC) get_proc("glUniform2ui");
	gleswUniform3ui = (PFNGLUNIFORM3UIPROC) get_proc("glUniform3ui");
	gleswUniform4ui = (PFNGLUNIFORM4UIPROC) get_proc("glUniform4ui");
	gleswUniform1uiv = (PFNGLUNIFORM1UIVPROC) get_proc("glUniform1uiv");
	gleswUniform2uiv = (PFNGLUNIFORM2UIVPROC) get_proc("glUniform2uiv");
	gleswUniform3uiv = (PFNGLUNIFORM3UIVPROC) get_proc("glUniform3uiv");
	gleswUniform4uiv = (PFNGLUNIFORM4UIVPROC) get_proc("glUniform4uiv");
	gleswClearBufferiv = (PFNGLCLEARBUFFERIVPROC) get_proc("glClearBufferiv");
	gleswClearBufferuiv = (PFNGLCLEARBUFFERUIVPROC) get_proc("glClearBufferuiv");
	gleswClearBufferfv = (PFNGLCLEARBUFFERFVPROC) get_proc("glClearBufferfv");
	gleswClearBufferfi = (PFNGLCLEARBUFFERFIPROC) get_proc("glClearBufferfi");
	gleswGetStringi = (PFNGLGETSTRINGIPROC) get_proc("glGetStringi");
	gleswCopyBufferSubData = (PFNGLCOPYBUFFERSUBDATAPROC) get_proc("glCopyBufferSubData");
	gleswGetUniformIndices = (PFNGLGETUNIFORMINDICESPROC) get_proc("glGetUniformIndices");
	gleswGetActiveUniformsiv = (PFNGLGETACTIVEUNIFORMSIVPROC) get_proc("glGetActiveUniformsiv");
	gleswGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC) get_proc("glGetUniformBlockIndex");
	gleswGetActiveUniformBlockiv = (PFNGLGETACTIVEUNIFORMBLOCKIVPROC) get_proc("glGetActiveUniformBlockiv");
	gleswGetActiveUniformBlockName = (PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC) get_proc("glGetActiveUniformBlockName");
	gleswUniformBlockBinding = (PFNGLUNIFORMBLOCKBINDINGPROC) get_proc("glUniformBlockBinding");
	gleswDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDPROC) get_proc("glDrawArraysInstanced");
	gleswDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC) get_proc("glDrawElementsInstanced");
	gleswFenceSync = (PFNGLFENCESYNCPROC) get_proc("glFenceSync");
	gleswIsSync = (PFNGLISSYNCPROC) get_proc("glIsSync");
	gleswDeleteSync = (PFNGLDELETESYNCPROC) get_proc("glDeleteSync");
	gleswClientWaitSync = (PFNGLCLIENTWAITSYNCPROC) get_proc("glClientWaitSync");
	gleswWaitSync = (PFNGLWAITSYNCPROC) get_proc("glWaitSync");
	gleswGetInteger64v = (PFNGLGETINTEGER64VPROC) get_proc("glGetInteger64v");
	gleswGetSynciv = (PFNGLGETSYNCIVPROC) get_proc("glGetSynciv");
	gleswGetInteger64i_v = (PFNGLGETINTEGER64I_VPROC) get_proc("glGetInteger64i_v");
	gleswGetBufferParameteri64v = (PFNGLGETBUFFERPARAMETERI64VPROC) get_proc("glGetBufferParameteri64v");
	gleswGenSamplers = (PFNGLGENSAMPLERSPROC) get_proc("glGenSamplers");
	gleswDeleteSamplers = (PFNGLDELETESAMPLERSPROC) get_proc("glDeleteSamplers");
	gleswIsSampler = (PFNGLISSAMPLERPROC) get_proc("glIsSampler");
	gleswBindSampler = (PFNGLBINDSAMPLERPROC) get_proc("glBindSampler");
	gleswSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC) get_proc("glSamplerParameteri");
	gleswSamplerParameteriv = (PFNGLSAMPLERPARAMETERIVPROC) get_proc("glSamplerParameteriv");
	gleswSamplerParameterf = (PFNGLSAMPLERPARAMETERFPROC) get_proc("glSamplerParameterf");
	gleswSamplerParameterfv = (PFNGLSAMPLERPARAMETERFVPROC) get_proc("glSamplerParameterfv");
	gleswGetSamplerParameteriv = (PFNGLGETSAMPLERPARAMETERIVPROC) get_proc("glGetSamplerParameteriv");
	gleswGetSamplerParameterfv = (PFNGLGETSAMPLERPARAMETERFVPROC) get_proc("glGetSamplerParameterfv");
	gleswVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC) get_proc("glVertexAttribDivisor");
	gleswBindTransformFeedback = (PFNGLBINDTRANSFORMFEEDBACKPROC) get_proc("glBindTransformFeedback");
	gleswDeleteTransformFeedbacks = (PFNGLDELETETRANSFORMFEEDBACKSPROC) get_proc("glDeleteTransformFeedbacks");
	gleswGenTransformFeedbacks = (PFNGLGENTRANSFORMFEEDBACKSPROC) get_proc("glGenTransformFeedbacks");
	gleswIsTransformFeedback = (PFNGLISTRANSFORMFEEDBACKPROC) get_proc("glIsTransformFeedback");
	gleswPauseTransformFeedback = (PFNGLPAUSETRANSFORMFEEDBACKPROC) get_proc("glPauseTransformFeedback");
	gleswResumeTransformFeedback = (PFNGLRESUMETRANSFORMFEEDBACKPROC) get_proc("glResumeTransformFeedback");
	gleswGetProgramBinary = (PFNGLGETPROGRAMBINARYPROC) get_proc("glGetProgramBinary");
	gleswProgramBinary = (PFNGLPROGRAMBINARYPROC) get_proc("glProgramBinary");
	gleswProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC) get_proc("glProgramParameteri");
	gleswInvalidateFramebuffer = (PFNGLINVALIDATEFRAMEBUFFERPROC) get_proc("glInvalidateFramebuffer");
	gleswInvalidateSubFramebuffer = (PFNGLINVALIDATESUBFRAMEBUFFERPROC) get_proc("glInvalidateSubFramebuffer");
	gleswTexStorage2D = (PFNGLTEXSTORAGE2DPROC) get_proc("glTexStorage2D");
	gleswTexStorage3D = (PFNGLTEXSTORAGE3DPROC) get_proc("glTexStorage3D");
	gleswGetInternalformativ = (PFNGLGETINTERNALFORMATIVPROC) get_proc("glGetInternalformativ");
}
