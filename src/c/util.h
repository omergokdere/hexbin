#pragma once

/* we populate the fields RTL but read LTR */
#define BITFIELD_INDEX_LTR(F, I, N) ((F) >> ((N) - 1 - (I)) & 1)
#define NIBBLEFIELD_INDEX_LTR(F, I, N) ((F) >> 4 * ((N) - 1 - (I)) & 0xf)

#define ARRAY_SIZE(X) (sizeof(X) / sizeof(X[0]))