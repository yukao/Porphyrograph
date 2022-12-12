#ifndef  __FREEGLUT_H__
#define  __FREEGLUT_H__

/*
 * freeglut.h
 *
 * The freeglut library include file
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PAWEL W. OLSZTA BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#define WACOM_TABLET

#include "freeglut_std.h"
#include "freeglut_ext.h"

#include "wintab.h"		// NOTE: get from wactab header package
extern BOOL pg_wacom_tabletActive;
void pg_wacom_tabletWintabInit( void );
void pg_wacom_tabletInit(HWND hWnd);

/*** END OF FILE ***/

#endif /* __FREEGLUT_H__ */
