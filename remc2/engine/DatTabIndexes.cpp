#include "DatTabIndexes.h"
//----- (0006EB90) --------------------------------------------------------
void CreateIndexes_6EB90(filearray_struct* fileArray)//24fb90
{
	for (uint32_t i = 0; (fileArray[i].begin_buffer) != &ZERO_BUFFER_PST2; i++)
	{
		if (x_WORD_180660_VGA_type_resolution & 1)
			sub_98709_create_index_dattab_power(*fileArray[i].begin_buffer, *fileArray[i].end_buffer, *fileArray[i].dat_buffer, *fileArray[i].posistruct);//279709 //1a6f44,1a6f86,1a6578
		else
			sub_9874D_create_index_dattab(*fileArray[i].begin_buffer, *fileArray[i].end_buffer, *fileArray[i].dat_buffer, *fileArray[i].posistruct);
	}
}
