//#include "windows.h"

#include "strmif.h"
#include "vfwmsgs.h"
#include "control.h"
#include "uuids.h"
#include "amvideo.h"
#include "olectl.h"
#include "initguid.h"
#include <dshow.h>

#include "assert.h"
#include "stdio.h"

#include "Grabber.h"


DEFINE_GUID( CLSID_Grabber_Filter, 0x17d93618, 0xe0a3, 0x4dde, 0x9b, 0x64, 0xea, 0x50, 0xa6, 0xfe, 0xa, 0x31);

// Application-defined message to notify app of filtergraph events
#define WM_GRAPHNOTIFY  WM_APP+1

#define grabberMAXDEVICES (16)

int   grabberNumDevices = 0;
char* grabberDevices[grabberMAXDEVICES] = {0,};
char* grabberLastDevice = NULL;



enum PLAYSTATE {Stopped, Paused, Running, Init};

struct GrabberConnection
{
	class grabberFilter *filter;
	IMediaSeeking *seekIFACE;
	IMediaControl * mediaControlIFACE;
	IGraphBuilder * graphBuilderIFACE;
	ICaptureGraphBuilder2 * captureGraphBuilderIFACE;
	PLAYSTATE g_psCurrent;
	IBaseFilter * cameraFilter;
};
GrabberConnection grabberConns[grabberMAXDEVICES] = {0,};
GrabberConnection conn;
	// DirectShow interfaces

HWND ghApp=0;
DWORD g_dwGraphRegister=0;

void Msg(TCHAR *szFormat, ...);

CRITICAL_SECTION grabberCriticalSection;
	// A critical section is used to mutex locks on the double buffering system

#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }


// These are custom DirectShow filters which, although ridiculously complicated
// due to the complex DirectShow paradigm, do nothing more than
// copy memory from the input pin into a system buffer!
//------------------------------------------------------------------------------------------

//GRRRRRRRRRRRRR
class grabberPin : public IMemInputPin, public IPin
{
	class grabberFilter *receiver;
  public:
	IPin *connected_to;
	AM_MEDIA_TYPE my_media_type;
	IMemAllocator *allocator;

	grabberPin(grabberFilter *receiver);
	STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	HRESULT BreakConnect();
	HRESULT SetMediaType(const AM_MEDIA_TYPE *media_type);
	HRESULT CheckMediaType(const AM_MEDIA_TYPE *media_type);
	HRESULT Active();
	HRESULT Inactive();
	STDMETHODIMP QueryId(LPWSTR *Id);
	STDMETHODIMP Receive(IMediaSample *media_sample);
	STDMETHODIMP Connect(IPin * pReceivePin, const AM_MEDIA_TYPE *media_type);
	STDMETHODIMP ReceiveConnection(IPin *connector, const AM_MEDIA_TYPE *media_type);
	STDMETHODIMP Disconnect();
	STDMETHODIMP ConnectedTo(IPin **pin_return);
	STDMETHODIMP ConnectionMediaType(AM_MEDIA_TYPE *media_type_return);
	STDMETHODIMP QueryPinInfo(PIN_INFO *pin_info_return);
	STDMETHODIMP QueryDirection(PIN_DIRECTION *pin_direction_return);
	STDMETHODIMP EnumMediaTypes(IEnumMediaTypes **enumerator_return);
	STDMETHODIMP QueryAccept(const AM_MEDIA_TYPE *media_type);
	STDMETHODIMP QueryInternalConnections(IPin **pins_return, ULONG *num_pins_return);
	STDMETHODIMP EndOfStream();
	STDMETHODIMP NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);
	STDMETHODIMP Notify(IBaseFilter * pSender, Quality q);
	STDMETHODIMP SetSink(IQualityControl * piqc);
	STDMETHODIMP GetAllocator(IMemAllocator ** ppAllocator);
	STDMETHODIMP NotifyAllocator(IMemAllocator * pAllocator, BOOL bReadOnly);
	STDMETHODIMP ReceiveMultiple (IMediaSample **pSamples, long nSamples, long *nSamplesProcessed);
	STDMETHODIMP ReceiveCanBlock();
	STDMETHODIMP BeginFlush();
	STDMETHODIMP EndFlush();
	STDMETHODIMP GetAllocatorRequirements(ALLOCATOR_PROPERTIES *pProps);
	BOOL IsConnected();
	IPin *GetConnected();
};

class grabberFilter : public IBaseFilter
{
  public:
	grabberFilter();
	~grabberFilter();

  public:
	int maxWidth, maxHeight;
	int width, height, stride, depth;
	char *incoming_image_buffers[2];
	int locks[2];
	int frameNums[2];
	int readLock;
	int frameTimings[16];
	unsigned int lastTime;
	const IUnknown *m_pUnknown;
	grabberPin *input_pin;
	IFilterGraph *filter_graph;
	WCHAR *name;
	LONG reference_count;

	int getAvgFrameTimings();
	HRESULT CheckMediaType(const AM_MEDIA_TYPE *media_type);
	HRESULT SetMediaType(const AM_MEDIA_TYPE *media_type);
	virtual HRESULT Receive(IMediaSample *incoming_sample);
	virtual HRESULT BreakConnect();
	STDMETHODIMP GetClassID(CLSID *class_id_return);
	STDMETHODIMP GetState(DWORD milliseconds, FILTER_STATE *state_return);
	STDMETHODIMP SetSyncSource(IReferenceClock *clock_return);
	STDMETHODIMP GetSyncSource(IReferenceClock **clock_return);
	STDMETHODIMP Stop();
	STDMETHODIMP Pause();
	STDMETHODIMP Run(REFERENCE_TIME tStart);
	STDMETHODIMP EnumPins(IEnumPins **enumerator_return);
	STDMETHODIMP FindPin(LPCWSTR name, IPin **pin_return);
	STDMETHODIMP QueryFilterInfo(FILTER_INFO * pInfo);
	STDMETHODIMP JoinFilterGraph(IFilterGraph *graph, LPCWSTR desired_name);
	STDMETHODIMP QueryVendorInfo(LPWSTR *vendor_info_return);
	STDMETHODIMP Register();
	STDMETHODIMP Unregister();
	STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
};

inline HRESULT grabberGetInterface(IUnknown *unknown, void **result)
{
	*result = unknown;
	unknown->AddRef();
	return NOERROR;
}

class grabberMediaTypeEnumerator : public IEnumMediaTypes
{
	int index;
	grabberPin *my_pin;
	LONG version;
	LONG reference_count;
  public:
	grabberMediaTypeEnumerator(grabberPin *my_pin, grabberMediaTypeEnumerator *enumerator_to_copy);
	virtual ~grabberMediaTypeEnumerator();
	STDMETHODIMP QueryInterface(REFIID riid, void **result);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP Next(ULONG num_media_types, AM_MEDIA_TYPE **media_types_return, ULONG *num_fetched_return);
	STDMETHODIMP Skip(ULONG numMediaTypes);
	STDMETHODIMP Reset();
	STDMETHODIMP Clone(IEnumMediaTypes **enumerator_return);
};

grabberPin::grabberPin(grabberFilter *_receiver)
{
	receiver = _receiver;
	connected_to = NULL;
	allocator = NULL;
}

STDMETHODIMP grabberPin::QueryInterface(REFIID riid, void **result)
{
	if (riid == IID_IMemInputPin) {		return grabberGetInterface((IMemInputPin *)this, result);	}
	if (riid == IID_IPin) {		        return grabberGetInterface((IPin *)this, result);	}
	if (riid == IID_IUnknown) {		    return grabberGetInterface((IUnknown *)(IPin *)this, result);	}
	*result = NULL;
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) grabberPin::AddRef()
{  
	return receiver->AddRef();	
};								  

STDMETHODIMP_(ULONG) grabberPin::Release()
{  
	return receiver->Release(); 
};

STDMETHODIMP grabberPin::Receive(IMediaSample *pSample)
{
	HRESULT hr = receiver->Receive(pSample);
	return hr;
}

HRESULT grabberPin::BreakConnect()
{
	HRESULT hr = receiver->BreakConnect();
	return hr;
}

STDMETHODIMP grabberPin::QueryId(LPWSTR *Id)
{
	LPCWSTR src = receiver->name;
	LPWSTR *result = Id;		

	DWORD name_len = sizeof(WCHAR) * (lstrlenW(src) + 1);

	LPWSTR dest = (LPWSTR)CoTaskMemAlloc(name_len);
	if (dest == NULL) return E_OUTOFMEMORY;

	*result = dest;
	CopyMemory(dest, src, name_len);

	return NOERROR;
}

HRESULT grabberPin::CheckMediaType(const AM_MEDIA_TYPE *media_type)
{
	return receiver->CheckMediaType(media_type);
}

HRESULT grabberPin::Active()
{
	return TRUE;
}

HRESULT grabberPin::Inactive()
{
	return FALSE;
}

HRESULT grabberPin::SetMediaType(const AM_MEDIA_TYPE *media_type)
{
	return receiver->SetMediaType(media_type);
}

STDMETHODIMP grabberPin::Connect(IPin *receive_pin, const AM_MEDIA_TYPE *media_type)
{
	return E_NOTIMPL;
}

STDMETHODIMP grabberPin::ReceiveConnection(IPin *connector_pin, const AM_MEDIA_TYPE *media_type)
{
	// Respond to a connection that an output pin is making.
	if (connected_to) return VFW_E_ALREADY_CONNECTED;
	// See whether media type is OK.
	HRESULT result = CheckMediaType(media_type);
	if (result != NOERROR)
	{
		BreakConnect();
		if (SUCCEEDED(result)||(result == E_FAIL)||(result == E_INVALIDARG))
			result = VFW_E_TYPE_NOT_ACCEPTED;	//!!!

		return result;
	}

	// Complete the connection.
	connected_to = connector_pin;
	connected_to->AddRef();
	result = SetMediaType(media_type);

	if (!SUCCEEDED(result))
	{
		connected_to->Release();
		connected_to = NULL;
		BreakConnect();
		return result;
	}

	return NOERROR;
}

STDMETHODIMP grabberPin::Disconnect()
{
	if (!connected_to) return S_FALSE;

	HRESULT result = BreakConnect();
	if (FAILED(result)) return result;

	connected_to->Release();
	connected_to = NULL;

	return S_OK;
}

STDMETHODIMP grabberPin::ConnectedTo(IPin **pin_return)
{
	IPin *pin = connected_to;
	*pin_return = pin;

	if (pin == NULL) return VFW_E_NOT_CONNECTED;

	pin->AddRef();
	return S_OK;
}

STDMETHODIMP grabberPin::ConnectionMediaType(AM_MEDIA_TYPE *media_type)
{
	if (IsConnected())
	{
		AM_MEDIA_TYPE *dest = media_type;
		AM_MEDIA_TYPE *source = &my_media_type;

		assert(source != dest);

		*dest = *source;
		if (source->cbFormat != 0)
		{
			assert(source->pbFormat != NULL);
			dest->pbFormat = (PBYTE)CoTaskMemAlloc(source->cbFormat);
			if (dest->pbFormat == NULL)
			{
				dest->cbFormat = 0;
				assert( 0 );
				return VFW_E_NOT_CONNECTED;
			}
			else
			{
				CopyMemory((PVOID)dest->pbFormat, (PVOID)source->pbFormat, dest->cbFormat);
			}
		}

		if (dest->pUnk != NULL) dest->pUnk->AddRef();
		return S_OK;
	}
	else
	{
		memset(media_type, 0, sizeof(*media_type));
		return VFW_E_NOT_CONNECTED;
	}
}

STDMETHODIMP grabberPin::QueryPinInfo(PIN_INFO *pin_info_return)
{
	pin_info_return->pFilter = receiver;
	if (receiver) receiver->AddRef();
	lstrcpynW( pin_info_return->achName, receiver->name, sizeof(pin_info_return->achName) / sizeof(WCHAR));
	pin_info_return->dir = PINDIR_INPUT;
	return NOERROR;
}

STDMETHODIMP grabberPin::QueryDirection(PIN_DIRECTION *direction_return)
{
	*direction_return = PINDIR_INPUT;
	return NOERROR;
}

STDMETHODIMP grabberPin::QueryAccept(const AM_MEDIA_TYPE *media_type)
{
	HRESULT result = CheckMediaType(media_type);
	if (FAILED(result)) return S_FALSE;
	return result;
}

STDMETHODIMP grabberPin::EnumMediaTypes(IEnumMediaTypes **enumerator_return)
{
	*enumerator_return = new grabberMediaTypeEnumerator(this, NULL);
	return S_OK;
}

STDMETHODIMP grabberPin::EndOfStream(void)
{
	return S_OK;
}

STDMETHODIMP grabberPin::SetSink(IQualityControl * piqc)
{
	return NOERROR;
}

STDMETHODIMP grabberPin::Notify(IBaseFilter *sender, Quality quality)
{
	UNREFERENCED_PARAMETER(quality);
	UNREFERENCED_PARAMETER(sender);
	return E_NOTIMPL;
}

STDMETHODIMP grabberPin::NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
{
	return S_OK;
}

STDMETHODIMP grabberPin::BeginFlush()
{
	return S_OK;
}

STDMETHODIMP grabberPin::EndFlush(void)
{
	return S_OK;
}

STDMETHODIMP grabberPin::ReceiveCanBlock()
{
	return S_FALSE;
}

STDMETHODIMP grabberPin::ReceiveMultiple(IMediaSample **samples, long num_samples, long *num_processed_return)
{
	HRESULT result = S_OK;
	int num_processed = 0;

	while (num_samples-- > 0)
	{
		 result = Receive(samples[num_processed]);

		 if (result != S_OK) break;

		 num_processed++;
	}

	*num_processed_return = num_processed;
	return result;
}

STDMETHODIMP grabberPin::GetAllocatorRequirements(ALLOCATOR_PROPERTIES *pProps)
{
	return E_NOTIMPL;
}

STDMETHODIMP grabberPin::NotifyAllocator(IMemAllocator *pAllocator, BOOL bReadOnly)
{
	if( allocator ) allocator->Release();
	allocator = NULL;
	allocator = pAllocator;
	pAllocator->AddRef();
	return NOERROR;
}

STDMETHODIMP grabberPin::GetAllocator(IMemAllocator **allocator_return)
{
	if (allocator == NULL)
	{
		HRESULT result = CoCreateInstance(CLSID_MemoryAllocator, 0, CLSCTX_INPROC_SERVER,IID_IMemAllocator, (void **)&allocator);
		if (FAILED(result)) return result;
	}
	assert(allocator != NULL);
	*allocator_return = allocator;
	allocator->AddRef();
	return NOERROR;
}

STDMETHODIMP grabberPin::QueryInternalConnections(IPin **pins_return, ULONG *num_pins_return)
{
	return E_NOTIMPL;
}

BOOL grabberPin::IsConnected()
{
	return (connected_to != NULL); 
}

IPin *grabberPin::GetConnected()
{
	return connected_to;
}

grabberMediaTypeEnumerator::grabberMediaTypeEnumerator(grabberPin *pPin, grabberMediaTypeEnumerator *pEnumMediaTypes)
{
	assert(pPin != NULL);

	reference_count = 1;
	my_pin = pPin;
	my_pin->AddRef();

	if (pEnumMediaTypes != NULL)
	{
		// This is a copy of another enumerator.
		index = pEnumMediaTypes->index;
		version = pEnumMediaTypes->version;
		return;
	}

	// This is a new enumerator, not a copy.
	index = 0;
	version = 0;
}

grabberMediaTypeEnumerator::~grabberMediaTypeEnumerator()
{
	my_pin->Release();
}


STDMETHODIMP grabberMediaTypeEnumerator::QueryInterface(REFIID riid, void **result)
{
	if (riid == IID_IEnumMediaTypes || riid == IID_IUnknown)
	{
		return grabberGetInterface((IEnumMediaTypes *) this, result);
	}
	else {
		*result = NULL;
		return E_NOINTERFACE;
	}
}

STDMETHODIMP_(ULONG) grabberMediaTypeEnumerator::AddRef()
{
	return InterlockedIncrement(&reference_count);
}

STDMETHODIMP_(ULONG) grabberMediaTypeEnumerator::Release()
{
	ULONG cRef = InterlockedDecrement(&reference_count);
	if (cRef == 0) {	delete this;	}
	return cRef;
}

STDMETHODIMP grabberMediaTypeEnumerator::Clone(IEnumMediaTypes **enumerator_return)
{
	HRESULT result = NOERROR;
	*enumerator_return = new grabberMediaTypeEnumerator(my_pin, this);
	if (*enumerator_return == NULL) result =  E_OUTOFMEMORY;

	return result;
}

STDMETHODIMP grabberMediaTypeEnumerator::Next(ULONG num_media_types, AM_MEDIA_TYPE **media_types_return, ULONG *num_fetched_return)
{
	// We don't have to enumerate media types; only output
	// pins need to do this.
	*num_fetched_return = 0;
	return S_FALSE;
}

STDMETHODIMP grabberMediaTypeEnumerator::Skip(ULONG num_to_skip)
{
	const int NUM_TYPES = 1;
	ULONG types_left = NUM_TYPES - index;
	if (num_to_skip > types_left) return S_FALSE;

	index += num_to_skip;
	return S_OK;
}

STDMETHODIMP grabberMediaTypeEnumerator::Reset()
{
	index = 0;
	version = 0;
	return NOERROR;
}


class grabberPinEnumerator : public IEnumPins
{
  public:
	grabberPinEnumerator(grabberFilter *receiver, grabberPinEnumerator *enumerator_to_copy);
	virtual ~grabberPinEnumerator();

	int index;
	grabberFilter *receiver;
	LONG reference_count;

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void **result);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IEnumPins
	STDMETHODIMP Next(ULONG num_pins, IPin **pins_return, ULONG *num_fetched_return);
	STDMETHODIMP Skip(ULONG num_to_skip);
	STDMETHODIMP Reset();
	STDMETHODIMP Clone(IEnumPins **result);
	STDMETHODIMP Refresh();
};

#pragma warning(disable:4355)
grabberFilter::grabberFilter() : m_pUnknown( reinterpret_cast<IUnknown *>(this) )
{
	reference_count = 1;
	name = NULL;
	filter_graph = NULL;
	input_pin = NULL;

	maxWidth = 320;
	maxHeight = 240;

	width = 0;
	height = 0;
	stride = 0;

	incoming_image_buffers[0] = 0;
	incoming_image_buffers[1] = 0;
	locks[0] = 0;
	locks[1] = 0;
	frameNums[0] = 0;
	frameNums[1] = 0;
	readLock = -1;

	memset( frameTimings, 0, sizeof(frameTimings) );
	lastTime = 0;

	input_pin = new grabberPin(this);
}

grabberFilter::~grabberFilter()
{
	if (incoming_image_buffers[0]) delete [] incoming_image_buffers[0];
	if (incoming_image_buffers[1]) delete [] incoming_image_buffers[1];
}

int grabberFilter::getAvgFrameTimings()
{
	int i;
	unsigned int sum = 0;
	for( i=0; i<sizeof(frameTimings)/sizeof(frameTimings[0]); i++ )
	{
		sum += frameTimings[i];
	}
	return int( sum / (unsigned)i );
}

STDMETHODIMP grabberFilter::QueryInterface(REFIID riid, void **result)
{
	if (riid == IID_IBaseFilter) {	      return grabberGetInterface((IBaseFilter *) this, result);}
	else if (riid == IID_IMediaFilter) {	return grabberGetInterface((IMediaFilter *) this, result);}
	else if (riid == IID_IPersist) {		  return grabberGetInterface((IPersist *) this, result);}
	else if (riid == IID_IAMovieSetup) {	return grabberGetInterface((IAMovieSetup *) this, result);}
	else if (riid == IID_IUnknown) {		  return grabberGetInterface((IUnknown *) this, result);	}
	else
	{
		*result = NULL;
		return E_NOINTERFACE;
	}
}

STDMETHODIMP_(ULONG) grabberFilter::AddRef()
{	
	return InterlockedIncrement(&reference_count);
};								  

STDMETHODIMP_(ULONG) grabberFilter::Release()
{  
	ULONG cRef = InterlockedDecrement(&reference_count);
	if (cRef == 0) {
		delete this;
	}
	return cRef;
};

HRESULT grabberFilter::CheckMediaType(const AM_MEDIA_TYPE *media_type)
{
	HRESULT   hr = E_FAIL;
	
	if (media_type->formattype != FORMAT_VideoInfo )
		return E_INVALIDARG;

	if( media_type->formattype == FORMAT_VideoInfo )
	{
		VIDEOINFOHEADER *pVih = reinterpret_cast<VIDEOINFOHEADER*>(media_type->pbFormat);
		if( pVih->bmiHeader.biWidth > maxWidth || pVih->bmiHeader.biHeight > maxHeight ) {
			return S_FALSE;
		}
	}
	
	if(		IsEqualGUID(media_type->majortype, MEDIATYPE_Video)
	   &&(IsEqualGUID(media_type->subtype, MEDIASUBTYPE_RGB24)|| IsEqualGUID(media_type->subtype, MEDIASUBTYPE_RGB8)	)
	   )
	{
		hr = S_OK;
	}
	
	return hr;
}

HRESULT grabberFilter::SetMediaType(const AM_MEDIA_TYPE *media_type)
{
	VIDEOINFO *pviBmp;
	pviBmp = (VIDEOINFO *)media_type->pbFormat;
	width  = pviBmp->bmiHeader.biWidth;
	height = abs(pviBmp->bmiHeader.biHeight);
	depth = pviBmp->bmiHeader.biBitCount / 8;
	stride = (width * depth + 3) & ~(3);

	if( incoming_image_buffers[0] ) {		delete incoming_image_buffers[0];	}
	if( incoming_image_buffers[1] ) {		delete incoming_image_buffers[1];	}

	incoming_image_buffers[0] = new char[width * height * depth];
	incoming_image_buffers[1] = new char[width * height * depth];
	locks[0] = 0;
	locks[1] = 0;
	frameNums[0] = 0;
	frameNums[1] = 0;
	readLock = -1;

	return S_OK;
}

HRESULT grabberFilter::BreakConnect()
{
	if (input_pin->IsConnected() == FALSE)
		return S_FALSE;

	return NOERROR;
}
/*
HRESULT grabberFilter::Receive(IMediaSample *incoming_sample)
{
	BYTE  *input_image;

	incoming_sample->GetPointer(&input_image);

	// DECIDE on which buffer.	Pick the oldest one that isn't locked
	EnterCriticalSection( &grabberCriticalSection );

	int which = 0;
	if( !locks[0] && !locks[1] ) {
		// Both are unlocked, pick the older
		which = frameNums[0] < frameNums[1] ? 0 : 1;
	}
	else if( locks[0] && locks[1] ) {
		// Both locked, do nothing
		LeaveCriticalSection( &grabberCriticalSection );
		return NULL;
	}
	else {
		// One is unlocked, other locked.  Pick the unlocked one.
		which = !locks[0] ? 0 : 1;
	}

	int nextFrameNum = max( frameNums[0], frameNums[1] ) + 1;

	char *dest_line_start = incoming_image_buffers[which];
	locks[which] = 1;

	LeaveCriticalSection( &grabberCriticalSection );

	int numTimingSlots = sizeof(frameTimings) / sizeof(frameTimings[0]);
	unsigned int now = timeGetTime();
	frameTimings[nextFrameNum % numTimingSlots] = now - lastTime;
	lastTime = now;

	BYTE *input_line_start = input_image + (height-1)*stride;
	int bpp = depth;

	if( bpp == 1 ) {
		for (int j = 0; j < height; j++) {
			BYTE *src = input_line_start;
			char *dest = dest_line_start;
			memcpy( dest, src, width );
			input_line_start -= stride;
			dest_line_start += width * bpp;
		}
	}
	else if( bpp == 2 ) {
		for (int j = 0; j < height; j++) {
			BYTE *src = input_line_start;
			char *dest = dest_line_start;
			memcpy( dest, src, width*2 );
			input_line_start -= stride;
			dest_line_start += width * bpp;
		}
	}
	else if( bpp = 3 ) {
		for (int j = 0; j < height; j++) {
			BYTE *src = input_line_start;
			char *dest = dest_line_start;
			int i;
			for (i = 0; i < width; i++) {
				dest[0] = src[2];
				dest[1] = src[1];
				dest[2] = src[0];
			
				src += 3;
				dest += bpp;
			}

			input_line_start -= stride;
			dest_line_start += width * bpp;
		}
	}

	EnterCriticalSection( &grabberCriticalSection );
	locks[which] = 0;
	frameNums[which] = nextFrameNum;
	LeaveCriticalSection( &grabberCriticalSection );
		
	return NOERROR;
}
*/
HRESULT grabberFilter::Receive(IMediaSample *incoming_sample)
{
	BYTE  *input_image;

	incoming_sample->GetPointer(&input_image);

	// DECIDE on which buffer.	Pick the oldest one that isn't locked
	EnterCriticalSection( &grabberCriticalSection );

	int which = 0;
	if( !locks[0] && !locks[1] ) {
		// Both are unlocked, pick the older
		which = frameNums[0] < frameNums[1] ? 0 : 1;
	}
	else if( locks[0] && locks[1] ) {
		// Both locked, do nothing
		LeaveCriticalSection( &grabberCriticalSection );
		return NULL;
	}
	else {
		// One is unlocked, other locked.  Pick the unlocked one.
		which = !locks[0] ? 0 : 1;
	}

	int nextFrameNum = max( frameNums[0], frameNums[1] ) + 1;

	char *dest_line_start = incoming_image_buffers[which];
	locks[which] = 1;

	LeaveCriticalSection( &grabberCriticalSection );

	int numTimingSlots = sizeof(frameTimings) / sizeof(frameTimings[0]);
	unsigned int now = timeGetTime();
	frameTimings[nextFrameNum % numTimingSlots] = now - lastTime;
	lastTime = now;

	int bpp = depth;

	BYTE *input_line_start = input_image + (height-1)*stride;

	if( bpp = 3 ) {
		int nbb = height*width*3;
		memcpy(dest_line_start,input_image,nbb);
	}

	EnterCriticalSection( &grabberCriticalSection );
	locks[which] = 0;
	frameNums[which] = nextFrameNum;
	LeaveCriticalSection( &grabberCriticalSection );
		
	return NOERROR;
}


STDMETHODIMP grabberFilter::JoinFilterGraph(IFilterGraph *graph, LPCWSTR desired_name)
{
	filter_graph = graph;

	if (name) {
		delete[] name;
		name = NULL;
	}

	if (desired_name) {
		DWORD len = lstrlenW(desired_name)+1;
		name = new WCHAR[len];
		assert(name);

		if (name) memcpy(name, desired_name, len * sizeof(WCHAR));
	}

	return NOERROR;
}

STDMETHODIMP grabberFilter::QueryVendorInfo(LPWSTR *vendor_info_return)
{
	UNREFERENCED_PARAMETER(vendor_info_return);
	return E_NOTIMPL;
}

STDMETHODIMP grabberFilter::Register()
{
	return S_FALSE;
}

STDMETHODIMP grabberFilter::Unregister()
{
	return S_FALSE;
}

STDMETHODIMP grabberFilter::SetSyncSource(IReferenceClock *clock_return)
{
	return NOERROR;
}

STDMETHODIMP grabberFilter::GetSyncSource(IReferenceClock **clock_return)
{
	*clock_return = NULL;
	return NOERROR;
}

STDMETHODIMP grabberFilter::Stop()
{
	// For some reason we might want to tell the receiver to stop,	 or something.
	HRESULT hr = NOERROR;
	return hr;
}

STDMETHODIMP grabberFilter::Pause()
{
	return S_OK;
}

STDMETHODIMP grabberFilter::Run(REFERENCE_TIME start_timex)
{
	return S_OK;
}

STDMETHODIMP grabberFilter::GetClassID(CLSID *class_id_return)
{
	*class_id_return = CLSID_Grabber_Filter;
	return NOERROR;
}

STDMETHODIMP grabberFilter::QueryFilterInfo(FILTER_INFO * pInfo)
{
	if (name) {	lstrcpynW(pInfo->achName, name, sizeof(pInfo->achName)/sizeof(WCHAR));	}
	else {	pInfo->achName[0] = L'\0';	}

	pInfo->pGraph = filter_graph;
	if (filter_graph) filter_graph->AddRef();

	return NOERROR;
}

STDMETHODIMP grabberFilter::FindPin(LPCWSTR Id, IPin **pin_return)
{
	grabberPin *pPin = input_pin;

	if (lstrcmpW(name, Id) == 0)
	{	// The pin's name is actually my name.
		*pin_return = pPin;
		pPin->AddRef();
		return S_OK;
	}

	*pin_return = NULL;
	return VFW_E_NOT_FOUND;
}

STDMETHODIMP grabberFilter::GetState(DWORD milliseconds, FILTER_STATE *state_return)
{
	*state_return = State_Running;
	return S_OK;
}

STDMETHODIMP grabberFilter::EnumPins(IEnumPins **ppEnum)
{
	*ppEnum = new grabberPinEnumerator(this, NULL);
	if (*ppEnum == NULL) return E_OUTOFMEMORY;
	return NOERROR;
}

grabberPinEnumerator::grabberPinEnumerator(grabberFilter *_receiver, grabberPinEnumerator *to_copy)
{
	reference_count = 1;
	index = 0;

	receiver = _receiver;
	receiver->AddRef();

	if (to_copy) index = to_copy->index;
}

grabberPinEnumerator::~grabberPinEnumerator()
{
	receiver->Release();
}

STDMETHODIMP grabberPinEnumerator::QueryInterface(REFIID riid, void **result)
{
	if (riid == IID_IEnumPins || riid == IID_IUnknown) {
		return grabberGetInterface((IEnumPins *)this, result);
	}
	else {
		*result = NULL;
		return E_NOINTERFACE;
	}
}

STDMETHODIMP_(ULONG) grabberPinEnumerator::AddRef()
{
	return InterlockedIncrement(&reference_count);
}

STDMETHODIMP_(ULONG) grabberPinEnumerator::Release()
{
	ULONG cRef = InterlockedDecrement(&reference_count);
	if (cRef == 0) delete this;  // !!! it is how COM works.
	return cRef;				 
}

STDMETHODIMP grabberPinEnumerator::Clone(IEnumPins **result)
{
	*result = new grabberPinEnumerator(receiver, this);
	if (*result == NULL) return E_OUTOFMEMORY;

	return NOERROR;
}

STDMETHODIMP grabberPinEnumerator::Next(ULONG num_pins, IPin **pins_return, ULONG *num_fetched_return)
{
	if (num_fetched_return != NULL) *num_fetched_return = 0;

	const int NUM_PINS = 1;
	int pins_left = NUM_PINS - index;
	if (pins_left <= 0) return S_FALSE;

	assert(index == 0);
	pins_return[0] = receiver->input_pin;
	receiver->input_pin->AddRef();
	index++;
	*num_fetched_return = 1;

	return NOERROR;
}

STDMETHODIMP grabberPinEnumerator::Skip(ULONG num_to_skip)
{
	const int NUM_PINS = 1;
	ULONG pins_left = NUM_PINS - index;
	if(num_to_skip > pins_left) return S_FALSE;

	index += num_to_skip;
	return S_OK;
}

STDMETHODIMP grabberPinEnumerator::Reset()
{
	index = 0;
	return S_OK;
}

STDMETHODIMP grabberPinEnumerator::Refresh()
{
	return Reset();
}


// grabber Internal Interfaces
//------------------------------------------------------------------------------------------

int grabberTraverseAndBindDevices( char *bindTo, void **boundFilter )
{
	// CLEAR the device list
	for( int i=0; i<grabberNumDevices; i++ ) {
		if( grabberDevices[i] ) {
			free( grabberDevices[i] );
			grabberDevices[i] = 0;
		}
	}
	grabberNumDevices = 0;

	// SETUP Microsoft COM
	CoInitialize(NULL);

	// INIT local
	HRESULT hr = 0;
	if( boundFilter ) *boundFilter = NULL;
	int success = 0;

	// ASSUME success if we are merely enumerating and not binding
	if( !bindTo ) success = 1;

	// INSTANCIATE a device enumerator
	ICreateDevEnum *pDeviceEnumerator;
	hr = CoCreateInstance( CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pDeviceEnumerator );
	if( hr != S_OK ) goto error;

	// INSTANCIATE a class enumerator on grabber inputs
	IEnumMoniker *pEnumMoniker;
	hr = pDeviceEnumerator->CreateClassEnumerator( CLSID_VideoInputDeviceCategory, &pEnumMoniker, 0 );
	if( hr != S_OK ) goto error;

	{ // ENUMARATE the devices and potentially bind to them
		pEnumMoniker->Reset();

		ULONG cFetched;
		IMoniker *pMoniker = NULL;
		while( hr = pEnumMoniker->Next(1, &pMoniker, &cFetched), hr==S_OK ) {
			IPropertyBag *pBag;
			char name[256] = {0,};

			hr = pMoniker->BindToStorage( 0, 0, IID_IPropertyBag, (void **)&pBag );
			if( hr == S_OK ) {
				VARIANT var;
				var.vt = VT_BSTR;
				hr = pBag->Read( L"FriendlyName", &var, NULL );
				if( hr == S_OK ) {
					WideCharToMultiByte( CP_ACP, 0, var.bstrVal, -1, name, 80, NULL, NULL );
					grabberDevices[grabberNumDevices++] = _strdup( name );
					assert( grabberNumDevices < grabberMAXDEVICES );
					SysFreeString( var.bstrVal );
				}
				hr = pBag->Release();
				if( bindTo && !*boundFilter ) {
					// If we are being asked to bind to a filter and we haven't already...
					if( !_stricmp( bindTo, name ) ) {
						hr = pMoniker->BindToObject( 0, 0, IID_IBaseFilter, boundFilter );
						success = hr == S_OK ? 1 : 0;
					}
				}
			}
			pMoniker->Release();
		}

	}

  error:
	if( pDeviceEnumerator )
		pDeviceEnumerator->Release();
	if( pEnumMoniker )
		pEnumMoniker->Release();
	return success;
}

int grabberDeviceNumFromName( char *deviceName )
{
	if( !deviceName )
		return -1;

	for( int i=0; i<grabberNumDevices; i++ )
	{
		if( !_stricmp(grabberDevices[i],deviceName) )
			return i;
	}
	return -1;
}



GrabberConnection *grabberGetConnectionFromName( char *deviceName )
{
	if( !deviceName ) {		deviceName = grabberLastDevice;	}
	if( !deviceName ) {		return NULL;	}

	for( int i=0; i<grabberNumDevices; i++ )
	{
		if( !_stricmp(grabberDevices[i],deviceName) )
			return &grabberConns[i];
	}
	return NULL;
}

void grabberRecursiveDestoryGraph( IFilterGraph *filterGraph, IBaseFilter *pf )
{
	IPin *pP, *pTo;
	ULONG u;
	IEnumPins *pins = NULL;
	PIN_INFO pininfo;
	HRESULT hr = pf->EnumPins(&pins);
	pins->Reset();
	while (hr == NOERROR) {
		hr = pins->Next(1, &pP, &u);
		if (hr == S_OK && pP) {
			pP->ConnectedTo(&pTo);
			if (pTo) {
				hr = pTo->QueryPinInfo(&pininfo);
				if (hr == NOERROR) {
					if (pininfo.dir == PINDIR_INPUT) {
						grabberRecursiveDestoryGraph(filterGraph,pininfo.pFilter);
						filterGraph->Disconnect(pTo);
						filterGraph->Disconnect(pP);
						filterGraph->RemoveFilter(pininfo.pFilter);
					}
					pininfo.pFilter->Release();
				}
				pTo->Release();
			}
			pP->Release();
		}
	}
	if (pins) {
		pins->Release();
	}
}


// grabber public interfaces
//--------------------------------------------------------------

char **grabberGetDevices( int *count )
{
	grabberTraverseAndBindDevices( NULL, NULL );
	if( count )
		*count = grabberNumDevices;

	return grabberDevices;
}

int grabberStartDevice( char *deviceName, int maxWidth, int maxHeight )
{
	InitializeCriticalSection( &grabberCriticalSection );

	if( !deviceName )
	{
		char **devs = grabberGetDevices();
		if( devs[0] )
			deviceName = devs[0];
		else
			return 0;
	}

	HRESULT hr;
	GrabberConnection *conn = grabberGetConnectionFromName( deviceName );
	if( !conn ) return 0;

	// CREATE the filter graph
	hr = CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void **)&conn->graphBuilderIFACE );
	if( !SUCCEEDED(hr) )
	{
		grabberShutdownDevice( deviceName );
		printf( "Grabber: CoCreateInstance 1 failure\n" );
		return 0;
	}
	assert( conn->graphBuilderIFACE );

	// CREATE the graph builder
	hr = CoCreateInstance( CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC, IID_ICaptureGraphBuilder2, (void **)&conn->captureGraphBuilderIFACE );
	if( !SUCCEEDED(hr) )
	{
		grabberShutdownDevice( deviceName );
		printf( "Grabber: CoCreateInstance 2 failure\n" );
		return 0;
	}
	assert( conn->captureGraphBuilderIFACE );

	// FIND the media control	 
	hr = conn->graphBuilderIFACE->QueryInterface( IID_IMediaControl,(LPVOID *) &conn->mediaControlIFACE );
	if( !SUCCEEDED(hr) )
	{
		grabberShutdownDevice( deviceName );
		printf( "Grabber: graphBuilderIFACE failure\n" );
		return 0;
	}
	assert( conn->mediaControlIFACE );

	// TELL the graph builder which filter graph it is working on
	hr = conn->captureGraphBuilderIFACE->SetFiltergraph( conn->graphBuilderIFACE );
	if( !SUCCEEDED(hr) )
	{
		grabberShutdownDevice( deviceName );
		printf( "Grabber: captureGraphBuilderIFACE failure\n" );
		return 0;
	}

	// BIND to the requested camera
	grabberTraverseAndBindDevices( deviceName, (void**)&conn->cameraFilter );

	// ADD capture filter to our graph.
	hr = conn->graphBuilderIFACE->AddFilter( conn->cameraFilter, L"Video Capture" );
	if( !SUCCEEDED(hr) )
	{
		printf( "Grabber: AddFilter 1 failure\n" );
		grabberShutdownDevice( deviceName );
		return 0;
	}

	// CREATE an instance of our capture filter which just grabs the data into memory
	conn->filter = new grabberFilter();
	conn->filter->maxWidth = maxWidth;
	conn->filter->maxHeight = maxHeight;

	// ATTACH our capture filter to the filterGraph
	hr = conn->graphBuilderIFACE->AddFilter( conn->filter, L"MEMORYRENDERER");
	if( !SUCCEEDED(hr) )
	{
		printf( "Grabber: AddFilter 2 failure\n" );
		grabberShutdownDevice( deviceName );
		return 0;
	}

	// CONNECT the video source to our capture filter (automatically creating intermediate filters?)
	hr = conn->captureGraphBuilderIFACE->RenderStream( &PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, conn->cameraFilter, NULL, conn->filter );
	if( !SUCCEEDED(hr) )
	{
		printf( "Grabber: RenderStream failure\n" );
		grabberShutdownDevice( deviceName );
		return 0;
	}

	// START capturing video
	hr = conn->mediaControlIFACE->Run();
	//if( !SUCCEEDED(hr) )
	//{
	//	printf( "Grabber: mediaControlIFACE failure\n" );
	//	grabberShutdownDevice( deviceName );
	//	return 0;
	//}

	grabberLastDevice = _strdup( deviceName );

	return 1;
}



void grabberShutdownDevice( char *deviceName ) {
	GrabberConnection *conn = grabberGetConnectionFromName( deviceName );
	if( conn )
	{
		if( conn->mediaControlIFACE )
		{
			conn->mediaControlIFACE->Stop();
			conn->mediaControlIFACE->Release();
			conn->mediaControlIFACE = NULL;
		}

		if( conn->filter )
		{
			conn->filter->Release();
			conn->filter = NULL;
		}

		if( conn->cameraFilter )
		{
			grabberRecursiveDestoryGraph( conn->graphBuilderIFACE, conn->cameraFilter );
			conn->cameraFilter->Release();
			conn->cameraFilter = NULL;
		}

		if( conn->captureGraphBuilderIFACE )
		{
			conn->captureGraphBuilderIFACE->Release();
			conn->captureGraphBuilderIFACE= NULL;
		}

		if( conn->graphBuilderIFACE )
		{
			conn->graphBuilderIFACE->Release();
			conn->graphBuilderIFACE = NULL;
		}

		if( conn->seekIFACE )
		{
			conn->seekIFACE->Release();
			conn->seekIFACE = NULL;
		}

		if( grabberLastDevice && !strcmp(grabberLastDevice,deviceName) )
		{
			free( grabberLastDevice );
			grabberLastDevice = 0;
		}
	}
}

void grabberShutdownAll()
{
	for( int i=0; i<grabberNumDevices; i++ )
	{
		grabberShutdownDevice( grabberDevices[i] );
	}
}

void grabberGetBitmapDesc( char *deviceName, int &w, int &h, int &d )
{
	w = 0;
	h = 0;
	d = 0;
	GrabberConnection *conn = grabberGetConnectionFromName( deviceName );
	if( conn )
	{
		if( conn->filter )
		{
			w = conn->filter->width;
			h = conn->filter->height;
			d = conn->filter->depth;
		}
	}
}

int grabberShowFilterPropertyPageModalDialog( char *deviceName )
{
	HRESULT hr = 0;
	int w0, h0, d0, w1, h1, d1;
	GrabberConnection *conn = grabberGetConnectionFromName( deviceName );
	if( conn )
	{
		// Don't do anything if we don't have a media control
		if( !conn->mediaControlIFACE ) return 0;

		// STORE old, see if it changed.
		grabberGetBitmapDesc( deviceName, w0, h0, d0 );

		// FETCH the camera filter's IID_ISpecifyPropertyPages interface
		ISpecifyPropertyPages *pispp = NULL;
		hr = conn->captureGraphBuilderIFACE->FindInterface( NULL, NULL, conn->cameraFilter, IID_ISpecifyPropertyPages, (void **)&pispp );
		if( !SUCCEEDED(hr) )
			return 0;

		// STOP the media
		//hr = conn->mediaControlIFACE->Stop();

		// FETCH the property page
		CAUUID caGUID;
		hr = pispp->GetPages(&caGUID);
		if( !SUCCEEDED(hr) ) {
			pispp->Release();
			hr = conn->mediaControlIFACE->Run();
			return 0;
		}
		pispp->Release();

		// CREATE the modal dialog box
		hr = OleCreatePropertyFrame(
			NULL, 0, 0, L"Filter",	1, (IUnknown **)&conn->cameraFilter,
			caGUID.cElems, caGUID.pElems, 0, 0, NULL
		);

		// RESUME the media
		//hr = conn->mediaControlIFACE->Run();

		// CHECK if the format changed
		grabberGetBitmapDesc( deviceName, w1, h1, d1 );

		if( w0!=w1 || h0!=h1 || d0!=d1 ) {
			return 1;
		}
	}
	return 0;
}

int grabberShowPinPropertyPageModalDialog( char *deviceName )
{
	HRESULT hr = 0;
	int w0, h0, d0, w1, h1, d1;
	GrabberConnection *conn = grabberGetConnectionFromName( deviceName );
	if( conn ) {
		// Don't do anything if we don't have a media control
		if( !conn->mediaControlIFACE ) return 0;

		// STORE old, see if it changed.
		grabberGetBitmapDesc( deviceName, w0, h0, d0 );

		// FETCH the output pin on the camera filter
		IPin *cameraPin = 0;
		hr = conn->captureGraphBuilderIFACE->FindPin( conn->cameraFilter, PINDIR_OUTPUT, &PIN_CATEGORY_CAPTURE, NULL, FALSE, 0, &cameraPin );
		if( !SUCCEEDED(hr) ) {
			return 0;
		}

		// FETCH the pin's IID_ISpecifyPropertyPages interface
		ISpecifyPropertyPages *pispp = NULL;
		hr = cameraPin->QueryInterface( IID_ISpecifyPropertyPages, (void **)&pispp );
		if( !SUCCEEDED(hr) ) {
			cameraPin->Release();
			return 0;
		}

		// STOP the media
		hr = conn->mediaControlIFACE->Stop();

		// FETCH the property page
		CAUUID caGUID;
		hr = pispp->GetPages(&caGUID);
		if( !SUCCEEDED(hr) ) {
			cameraPin->Release();
			pispp->Release();
			hr = conn->mediaControlIFACE->Run();
			return 0;
		}
		pispp->Release();

		// CREATE the modal dialog
		hr = OleCreatePropertyFrame(NULL,0,0,L"Filter",1,(IUnknown **)&cameraPin,caGUID.cElems,caGUID.pElems,0,0,NULL);
		cameraPin->Release();

		// RESUME the media
		hr = conn->mediaControlIFACE->Run();

		// CHECK if the format changed
		grabberGetBitmapDesc( deviceName, w1, h1, d1 );
		if( w0!=w1 || h0!=h1 || d0!=d1 )
		{
			return 1;
		}
	}

	return 0;
}

char *grabberLockNewest( char *deviceName, int *frameNumber )	//modif
{
	int idev = grabberDeviceNumFromName(deviceName );
	return grabberLockNewest( idev , frameNumber );
}

char *grabberLockNewest( int deviceNum, int *frameNumber ) //ajout
{
	if((deviceNum<0)||(deviceNum>=grabberNumDevices))
		return NULL;

	printf( "Grabber: knows Device #%d\n" , deviceNum );

	GrabberConnection *conn = &grabberConns[deviceNum];

	if( (conn!=NULL)&&(conn->filter!=NULL) )
	{
		EnterCriticalSection( &grabberCriticalSection );

		// WHICH one is newer and unlocked?
		int which = 0;
		if( !conn->filter->locks[0] && !conn->filter->locks[1] )
		{
			// Both are unlocked, pick the newer
			which = conn->filter->frameNums[0] > conn->filter->frameNums[1] ? 0 : 1;
		}
		else if( conn->filter->locks[0] && conn->filter->locks[1] )
		{
			// Both locked, do nothing
			LeaveCriticalSection( &grabberCriticalSection );
			conn->filter->readLock = -1;
			printf("Grabber: Double lock\n" );
			return NULL;
		}
		else
		{
			// One is unlocked, other locked.  Pick the unlocked one.
			which = !conn->filter->locks[0] ? 0 : 1;
		}

		if( frameNumber )
		{
			//if( *frameNumber >= conn->filter->frameNums[which] ) {
			//	// This isn't any newer than the one we already have
			//	LeaveCriticalSection( &grabberCriticalSection );
			//	conn->filter->readLock = -1;
			//	printf("Grabber: No new frame\n" );
			//	return NULL;
			//}
			//else
			//{
				*frameNumber = conn->filter->frameNums[which];
				printf("Grabber: Frame no %d\n" , *frameNumber );
			//}
		}

		conn->filter->locks[which] = 1;
		conn->filter->readLock = which;
		LeaveCriticalSection( &grabberCriticalSection );

		return conn->filter->incoming_image_buffers[which];
	}
	printf("Grabber: NULL Connection or Filter\n" );
	return NULL;
}

void grabberUnlock( char *deviceName )
{
	GrabberConnection *conn = grabberGetConnectionFromName( deviceName );
	if( conn )
	{
		EnterCriticalSection( &grabberCriticalSection );
		if( conn->filter->readLock != -1 )
		{
			conn->filter->locks[ conn->filter->readLock ] = 0;
		}
		LeaveCriticalSection( &grabberCriticalSection );
	}
}

int grabberGetAvgFrameTimeInMils( char *deviceName )
{
	GrabberConnection *conn = grabberGetConnectionFromName( deviceName );
	if( conn )
	{
		return conn->filter->getAvgFrameTimings();
	}
	return 0;
}

void Msg(TCHAR *szFormat, ...)
{
    TCHAR szBuffer[1024];  // Large buffer for long filenames or URLs
    const size_t NUMCHARS = sizeof(szBuffer) / sizeof(szBuffer[0]);
    const int LASTCHAR = NUMCHARS - 1;

    // Format the input string
    va_list pArgs;
    va_start(pArgs, szFormat);

    // Use a bounded buffer size to prevent buffer overruns.  Limit count to
    // character size minus one to allow for a NULL terminating character.
    (void)StringCchVPrintf(szBuffer, NUMCHARS - 1, szFormat, pArgs);
    va_end(pArgs);

    // Ensure that the formatted string is NULL-terminated
    szBuffer[LASTCHAR] = TEXT('\0');

    MessageBox(NULL, szBuffer, TEXT("PlayCap Message"), MB_OK | MB_ICONERROR);
}


HRESULT GetInterfaces(void)
{
    HRESULT hr;

	(conn.mediaControlIFACE) = NULL;
	(conn.graphBuilderIFACE) = NULL;
	(conn.captureGraphBuilderIFACE) = NULL;
	(conn.g_psCurrent) = Stopped;
	(conn.cameraFilter)=NULL;

	// Create the filter graph
    hr = CoCreateInstance (CLSID_FilterGraph, NULL, CLSCTX_INPROC,
                           IID_IGraphBuilder, (void **) &(conn.graphBuilderIFACE));
    if (FAILED(hr))
        return hr;

    // Create the capture graph builder
    hr = CoCreateInstance (CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC,
                           IID_ICaptureGraphBuilder2, (void **) &(conn.captureGraphBuilderIFACE));
    if (FAILED(hr))
        return hr;
    
    // Obtain interfaces for media control and Video Window
    hr = (conn.graphBuilderIFACE)->QueryInterface(IID_IMediaControl,(LPVOID *) &(conn.mediaControlIFACE));
    if (FAILED(hr))
        return hr;

    return hr;
}

HRESULT FindCaptureDevice(IBaseFilter ** pcameraFilter)
{
    HRESULT hr = S_OK;
    IBaseFilter * pSrc = NULL;
    IMoniker* pMoniker =NULL;
    ICreateDevEnum *pDevEnum =NULL;
    IEnumMoniker *pClassEnum = NULL;

    if (!pcameraFilter)
	{
        return E_POINTER;
	}
   
    // Create the system device enumerator
    hr = CoCreateInstance (CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
                           IID_ICreateDevEnum, (void **) &pDevEnum);
    if (FAILED(hr))
    {
        Msg(TEXT("Couldn't create system enumerator!  hr=0x%x"), hr);
    }

    // Create an enumerator for the video capture devices

	if (SUCCEEDED(hr))
	{
	    hr = pDevEnum->CreateClassEnumerator (CLSID_VideoInputDeviceCategory, &pClassEnum, 0);
		if (FAILED(hr))
		{
			Msg(TEXT("Couldn't create class enumerator!  hr=0x%x"), hr);
	    }
	}

	if (SUCCEEDED(hr))
	{
		// If there are no enumerators for the requested type, then 
		// CreateClassEnumerator will succeed, but pClassEnum will be NULL.
		if (pClassEnum == NULL)
		{
			MessageBox(ghApp,TEXT("No video capture device was detected.\r\n\r\n")
				TEXT("This sample requires a video capture device, such as a USB WebCam,\r\n")
				TEXT("to be installed and working properly.  The sample will now close."),
				TEXT("No Video Capture Hardware"), MB_OK | MB_ICONINFORMATION);
			hr = E_FAIL;
		}
	}

    // Use the first video capture device on the device list.
    // Note that if the Next() call succeeds but there are no monikers,
    // it will return S_FALSE (which is not a failure).  Therefore, we
    // check that the return code is S_OK instead of using SUCCEEDED() macro.

	if (SUCCEEDED(hr))
	{
		hr = pClassEnum->Next (1, &pMoniker, NULL);
		if (hr == S_FALSE)
		{
	        Msg(TEXT("Unable to access video capture device!"));   
			hr = E_FAIL;
		}
	}

	if (SUCCEEDED(hr))
    {
        // Bind Moniker to a filter object
        hr = pMoniker->BindToObject(0,0,IID_IBaseFilter, (void**)&pSrc);
        if (FAILED(hr))
        {
            Msg(TEXT("Couldn't bind moniker to filter object!  hr=0x%x"), hr);
        }
    }

    // Copy the found filter pointer to the output parameter.
	if (SUCCEEDED(hr))
	{
	    *pcameraFilter = pSrc;
		(*pcameraFilter)->AddRef();
	}

	SAFE_RELEASE(pSrc);
    SAFE_RELEASE(pMoniker);
    SAFE_RELEASE(pDevEnum);
    SAFE_RELEASE(pClassEnum);

    return hr;
}

HRESULT CaptureVideo( int maxWidth, int maxHeight )
{
    HRESULT hr;

    // Get DirectShow interfaces
    hr = GetInterfaces();
    if (FAILED(hr))
    {
        Msg(TEXT("Failed to get video interfaces!  hr=0x%x"), hr);
        return hr;
    }

    // Attach the filter graph to the capture graph
    hr = (conn.captureGraphBuilderIFACE)->SetFiltergraph((conn.graphBuilderIFACE));
    if (FAILED(hr))
    {
        Msg(TEXT("Failed to set capture filter graph!  hr=0x%x"), hr);
        return hr;
    }

    // Use the system device enumerator and class enumerator to find
    // a video capture/preview device, such as a desktop USB video camera.
    hr = FindCaptureDevice(&(conn.cameraFilter));
    if (FAILED(hr))
    {
        // Don't display a message because FindCaptureDevice will handle it
        return hr;
    }
   
    // Add Capture filter to our graph.
    hr = (conn.graphBuilderIFACE)->AddFilter((conn.cameraFilter), L"Video Capture");
    if (FAILED(hr))
    {
        Msg(TEXT("Couldn't add the capture filter to the graph!  hr=0x%x\r\n\r\n") 
            TEXT("If you have a working video capture device, please make sure\r\n")
            TEXT("that it is connected and is not being used by another application.\r\n\r\n")
            TEXT("The sample will now close."), hr);
        (conn.cameraFilter)->Release();
        return hr;
    }

    //// CREATE an instance of our capture filter which just grabs the data into memory
    //conn.filter = new grabberFilter();
    //conn.filter->maxWidth = maxWidth;
    //conn.filter->maxHeight = maxHeight;

    //// ATTACH our capture filter to the filterGraph
    //hr = conn.graphBuilderIFACE->AddFilter( conn.filter, L"MEMORYRENDERER");
    //if( !SUCCEEDED(hr) )
    //{
	   // printf( "Grabber: AddFilter 2 failure\n" );
	   // return 0;
    //}

	// Render the preview pin on the video capture filter
    // Use this instead of (conn.graphBuilderIFACE)->RenderFile
    hr = (conn.captureGraphBuilderIFACE)->RenderStream (&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,
                                   (conn.cameraFilter), NULL, NULL);
    if (FAILED(hr))
    {
        Msg(TEXT("Couldn't render the video capture stream.  hr=0x%x\r\n")
            TEXT("The capture device may already be in use by another application.\r\n\r\n")
            TEXT("The sample will now close."), hr);
        (conn.cameraFilter)->Release();
        return hr;
    }

    // Now that the filter has been added to the graph and we have
    // rendered its stream, we can release this reference to the filter.
    (conn.cameraFilter)->Release();

#ifdef register_filtergraph
    // add our graph to the running object table, which will allow
    // the graphedit application to "spy" on our graph
    hr = addgraphtorot((conn.graphbuilderiface), &g_dwgraphregister);
    if (failed(hr))
    {
        msg(text("failed to register filter graph with rot!  hr=0x%x"), hr);
        g_dwgraphregister = 0;
    }
#endif

    // Start previewing video data
    hr = (conn.mediaControlIFACE)->Run();
    if (FAILED(hr))
    {
        Msg(TEXT("Couldn't run the graph!  hr=0x%x"), hr); // 800705aa
        return hr;
    }

    // Remember current state
    (conn.g_psCurrent) = Running;
        
    return S_OK;
}
