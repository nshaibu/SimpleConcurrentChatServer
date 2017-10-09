/*===========================================================================================
# Copyright (C) 2017 Nafiu Shaibu.
# Purpose: 
#-------------------------------------------------------------------------------------------
# This is a free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 3 of the License, or (at your option) 
# any later version.

# This is distributed in the hopes that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
# Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

#===========================================================================================*/

#include <unistd.h>
#include <time.h>

#include "./include/func_pointer.h"


int ip_addr_valid(char *ip) {
	char temp[4];
	int i=0, z=0, var, j;
	
	if (ip == NULL) return -1;
	
	memset(temp, '\0', 4);
	for (j=0; j<4; j++) {
		while (ip[z] != '.' && ip[z] != '\0') { temp[i++] = ip[z++]; if (i>3) return -1;  }
		temp[i]='\0';
		
		var = atoi(temp);
		if (var > 255 || var < 0 || (j == 0 && var == 0) ) return -1;
		i=0;
		if (ip[z] != '\0') z++;
	}
	
	return (j<4 || j>4)? -1 : 1;
}
