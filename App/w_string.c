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
#include <stddef.h>

/* Private types ------------------------------------------------------------*/
/* Private constants --------------------------------------------------------*/
/* Private macro ------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
/* Private function prototypes ----------------------------------------------*/

void *__wrap_memcpy(void *dst, const void *src, size_t sz)
{
   void *temp = dst;

   while(sz)
   {
      *(unsigned char *)temp = *(unsigned char *)src;
      src++;
      temp++;
      sz--;
   }

   return(dst);
}
