/**
  ******************************************************************************
  * @file    ${file_name} 
  * @author  ${user}
  * @version 
  * @date    ${date}
  * @brief   
  ******************************************************************************
  * @attention
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
  *
  ******************************************************************************
  */

/* Includes -----------------------------------------------------------------*/
#include <sys/reent.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "ff.h"

/* Private types ------------------------------------------------------------*/
/* Private constants --------------------------------------------------------*/
/* Private macro ------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
/* Private function prototypes ----------------------------------------------*/

FILE *__wrap_fopen(const char *filename, const char *mode)
{
	FIL *fp = malloc(sizeof(FIL));
	unsigned char fmode;
	
	if(strcmp(mode, "rb") == 0)
	{
		fmode = FA_READ;
	}
	else
	{
		fmode = FA_CREATE_ALWAYS | FA_WRITE;
	}
	
	if (f_open (fp, filename, fmode) == FR_OK)
	{
		return ((FILE *) fp);
	}
	
	free(fp);
	return (0);
}

int __wrap_fclose(FILE *stream)
{
	FIL *fp = (FIL *) stream;
	
	if (f_close (fp) == FR_OK)
	{
		free(fp);
		return (0);
	}
	
	return (-1);
}

size_t __wrap_fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	FIL *fp = (FIL *) stream;
	unsigned int read_count;
	unsigned int buf_count = size * nmemb;
	
	f_read (fp, ptr, buf_count, &read_count);
	
	if(read_count < buf_count)
	{
		return (-1);
	}
	
	return (read_count / size);
}

size_t __wrap_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	FIL *fp = (FIL *) stream;
	unsigned int write_count;
	unsigned int buf_count = size * nmemb;
	
	f_write (fp, ptr, buf_count, &write_count);
	
	if(write_count < buf_count)
	{
		return (-1);
	}
	
	return (write_count / size);
}

int __wrap_fseek(FILE *stream, long int offset, int whence)
{
	FIL *fp = (FIL *) stream;
	long temp_offset;

	switch(whence)
	{
		case 0:
			temp_offset = offset;
			break;
		case 1:
			temp_offset = fp->fptr + offset;
			break;
		case 2:
			temp_offset = fp->fsize - offset;
			break;
	}

	if(f_lseek (fp, temp_offset) == FR_OK)
	{
		return (0);
	}
	
	return (-1);
}

int __wrap_remove(const char *filename)
{
	if(f_unlink (filename) == FR_OK)
	{
		return (0);
	}
	
	return (-1);
}

long int __wrap_ftell(FILE *stream)
{
	FIL *fp = (FIL *) stream;

	return (fp->fptr);
}
