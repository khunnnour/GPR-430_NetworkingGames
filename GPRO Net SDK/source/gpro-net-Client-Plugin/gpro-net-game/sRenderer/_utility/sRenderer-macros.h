/*
	Copyright 2011-2021 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein

	sRenderer-macros.h
	Declarations of helpful macros.

	********************************************
	*** Declarations of utility macros.      ***
	********************************************
*/

#ifndef _GPRO_NET_SRENDERER_MACROS_H_
#define _GPRO_NET_SRENDERER_MACROS_H_


//-----------------------------------------------------------------------------
// helpers for toggling and looping controls

#define rendererCtrlToggle(value)											(value = !value)
#define rendererCtrlIncLoop(value, maximum)									(value = (value + 1) % maximum)
#define rendererCtrlDecLoop(value, maximum)									(value = (value + maximum - 1) % maximum)
#define rendererCtrlIncCap(value, maximum)									(value = value < maximum ? value + 1 : value)
#define rendererCtrlDecCap(value, minimum)									(value = value > minimum ? value - 1 : value)
#define rendererCtrlIncClamp(value, maximum, minimum)						(value = value < maximum && value > minimum ? maximum : value)
#define rendererCtrlDecClamp(value, maximum, minimum)						(value = value > minimum && value < maximum ? minimum : value)

#define rendererCtrlCaseToggle(value, ctrl)									case ctrl: rendererCtrlToggle(value); break
#define rendererCtrlCaseIncLoop(value, maximum, ctrl)						case ctrl: rendererCtrlIncLoop(value, maximum); break
#define rendererCtrlCaseDecLoop(value, maximum, ctrl)						case ctrl: rendererCtrlDecLoop(value, maximum); break
#define rendererCtrlCaseIncCap(value, maximum, ctrl)						case ctrl: rendererCtrlIncCap(value, maximum); break
#define rendererCtrlCaseDecCap(value, minimum, ctrl)						case ctrl: rendererCtrlDecCap(value, minimum); break

#define rendererCtrlCasesLoop(value, maximum, ctrlInc, ctrlDec)				rendererCtrlCaseIncLoop(value, maximum, ctrlInc);	rendererCtrlCaseDecLoop(value, maximum, ctrlDec)
#define rendererCtrlCasesCap(value, maximum, minimum, ctrlInc, ctrlDec)		rendererCtrlCaseIncCap(value, maximum, ctrlInc);	rendererCtrlCaseDecCap(value, minimum, ctrlDec)

#define rendererArrayLen(arr)												(sizeof(arr) / sizeof(*arr))


//-----------------------------------------------------------------------------


#endif	// !_GPRO_NET_SRENDERER_MACROS_H_