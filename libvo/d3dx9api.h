/* d3dxApi.h -- D3DX Shader API */

#ifndef __D3DX9SHADER_H__
#define __D3DX9SHADER_H__

///////////////////////////////////////////////////////////////////////////
// ID3DXBuffer:
// ------------
// The buffer object is used by D3DX to return arbitrary size data.
//
// GetBufferPointer -
//    Returns a pointer to the beginning of the buffer.
//
// GetBufferSize -
//    Returns the size of the buffer, in bytes.
///////////////////////////////////////////////////////////////////////////


#undef INTERFACE
#define INTERFACE ID3DXBuffer

DECLARE_INTERFACE_(ID3DXBuffer, IUnknown)
{
    // IUnknown
    STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    // ID3DXBuffer
    STDMETHOD_(LPVOID, GetBufferPointer)(THIS) PURE;
    STDMETHOD_(DWORD, GetBufferSize)(THIS) PURE;
};

typedef interface ID3DXBuffer *LPD3DXBUFFER;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define ID3DXBuffer_QueryInterface(p,riid,ppvObject) (p)->lpVtbl->QueryInterface(p,riid,ppvObject)
#define ID3DXBuffer_AddRef(p) (p)->lpVtbl->AddRef(p)
#define ID3DXBuffer_Release(p) (p)->lpVtbl->Release(p)
#define ID3DXBuffer_GetBufferPointer(p) (p)->lpVtbl->GetBufferPointer(p)
#define ID3DXBuffer_GetBufferSize(p) (p)->lpVtbl->GetBufferSize(p)
#else  /* !defined(__cplusplus) || defined(CINTERFACE) */
#define ID3DXBuffer_QueryInterface(p,riid,ppvObject) (p)->QueryInterface(p,riid,ppvObject)
#define ID3DXBuffer_AddRef(p) (p)->AddRef(p)
#define ID3DXBuffer_Release(p) (p)->Release(p)
#define ID3DXBuffer_GetBufferPointer(p) (p)->GetBufferPointer(p)
#define ID3DXBuffer_GetBufferSize(p) (p)->GetBufferSize(p)
#endif  /* !defined(__cplusplus) || defined(CINTERFACE) */

//----------------------------------------------------------------------------
// D3DXMACRO:
// ----------
// Preprocessor macro definition.  The application pass in a NULL-terminated
// array of this structure to various D3DX APIs.  This enables the application
// to #define tokens at runtime, before the file is parsed.
//----------------------------------------------------------------------------

typedef struct _D3DXMACRO
{
    LPCSTR Name;
    LPCSTR Definition;

} D3DXMACRO, *LPD3DXMACRO;

//----------------------------------------------------------------------------
// D3DXSHADER flags:
// -----------------
// D3DXSHADER_DEBUG
//   Insert debug file/line/type/symbol information.
//
// D3DXSHADER_SKIPVALIDATION
//   Do not validate the generated code against known capabilities and
//   constraints.  This option is only recommended when compiling shaders
//   you KNOW will work.  (ie. have compiled before without this option.)
//   Shaders are always validated by D3D before they are set to the device.
//
// D3DXSHADER_SKIPOPTIMIZATION 
//   Instructs the compiler to skip optimization steps during code generation.
//   Unless you are trying to isolate a problem in your code using this option 
//   is not recommended.
//
// D3DXSHADER_PACKMATRIX_ROWMAJOR
//   Unless explicitly specified, matrices will be packed in row-major order
//   on input and output from the shader.
//
// D3DXSHADER_PACKMATRIX_COLUMNMAJOR
//   Unless explicitly specified, matrices will be packed in column-major 
//   order on input and output from the shader.  This is generally more 
//   efficient, since it allows vector-matrix multiplication to be performed
//   using a series of dot-products.
//
// D3DXSHADER_PARTIALPRECISION
//   Force all computations in resulting shader to occur at partial precision.
//   This may result in faster evaluation of shaders on some hardware.
//
// D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT
//   Force compiler to compile against the next highest available software
//   target for vertex shaders.  This flag also turns optimizations off, 
//   and debugging on.  
//
// D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT
//   Force compiler to compile against the next highest available software
//   target for pixel shaders.  This flag also turns optimizations off, 
//   and debugging on.
//
// D3DXSHADER_NO_PRESHADER
//   Disables Preshaders. Using this flag will cause the compiler to not 
//   pull out static expression for evaluation on the host cpu
//
// D3DXSHADER_AVOID_FLOW_CONTROL
//   Hint compiler to avoid flow-control constructs where possible.
//
// D3DXSHADER_PREFER_FLOW_CONTROL
//   Hint compiler to prefer flow-control constructs where possible.
//
//----------------------------------------------------------------------------

#define D3DXSHADER_DEBUG                    (1 << 0)
#define D3DXSHADER_SKIPVALIDATION           (1 << 1)
#define D3DXSHADER_SKIPOPTIMIZATION         (1 << 2)
#define D3DXSHADER_PACKMATRIX_ROWMAJOR      (1 << 3)
#define D3DXSHADER_PACKMATRIX_COLUMNMAJOR   (1 << 4)
#define D3DXSHADER_PARTIALPRECISION         (1 << 5)
#define D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT  (1 << 6)
#define D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT  (1 << 7)
#define D3DXSHADER_NO_PRESHADER             (1 << 8)
#define D3DXSHADER_AVOID_FLOW_CONTROL       (1 << 9)
#define D3DXSHADER_PREFER_FLOW_CONTROL      (1 << 10)

//----------------------------------------------------------------------------
// D3DXINCLUDE_TYPE:
//----------------------------------------------------------------------------

typedef enum _D3DXINCLUDE_TYPE
{
    D3DXINC_LOCAL,
    D3DXINC_SYSTEM,

    // force 32-bit size enum
    D3DXINC_FORCE_DWORD = 0x7fffffff

} D3DXINCLUDE_TYPE, *LPD3DXINCLUDE_TYPE;


//----------------------------------------------------------------------------
// ID3DXInclude:
// -------------
// This interface is intended to be implemented by the application, and can
// be used by various D3DX APIs.  This enables application-specific handling
// of #include directives in source files.
//
// Open()
//    Opens an include file.  If successful, it should fill in ppData and
//    pBytes.  The data pointer returned must remain valid until Close is
//    subsequently called.
// Close()
//    Closes an include file.  If Open was successful, Close is guaranteed
//    to be called before the API using this interface returns.
//----------------------------------------------------------------------------


#undef INTERFACE
#define INTERFACE ID3DXInclude

DECLARE_INTERFACE(ID3DXInclude)
{
    STDMETHOD(Open)(THIS_ D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes) PURE;
    STDMETHOD(Close)(THIS_ LPCVOID pData) PURE;
};

typedef interface ID3DXInclude *LPD3DXINCLUDE;

typedef HRESULT (WINAPI * D3DXCompileShaderPtr) (
	LPCSTR		pSrcData,
	UINT		SrcDataLen,
	CONST D3DXMACRO* pDefines,
	LPD3DXINCLUDE	pInclude,
	LPCSTR		pFunctionName,
	LPCSTR		pProfile,
	DWORD		Flags,
	LPD3DXBUFFER*	ppShader,
	LPD3DXBUFFER*	ppErrorMsgs,
	void* ppConstantTable);

#endif //__D3DX9SHADER_H__



