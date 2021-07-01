#ifndef __MIXER_H
#define __MIXER_H

#include "sys.h"
#include "flags.h"

extern u16 mixerResult[4];

void MixerInit(void);
void MixerUpdate(void);

#endif
