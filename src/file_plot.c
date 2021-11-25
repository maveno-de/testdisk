/*

    File: file_plot.c

    This software is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
  
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
  
    You should have received a copy of the GNU General Public License along
    with this program; if not, write the Free Software Foundation, Inc., 51
    Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

 */

#if !defined(SINGLE_FORMAT) || defined(SINGLE_FORMAT_plot)
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include <stdio.h>
#include "types.h"
#include "filegen.h"
#include <time.h>

/*@ requires valid_register_header_check(file_stat); */
static void register_header_check_plot(file_stat_t *file_stat);

const file_hint_t file_hint_plot = {
  .extension = "plot",
  .description = "Chia plot file",
  .max_filesize = PHOTOREC_MAX_FILE_SIZE,
  .recover = 1,
  .enable_by_default = 1,
  .register_header_check = &register_header_check_plot
};

/*@
  @ requires file_recovery->file_rename==&file_rename_plot;
  @ requires valid_file_rename_param(file_recovery);
  @ ensures  valid_file_rename_result(file_recovery);
  @*/
static void file_rename_plot(file_recovery_t *file_recovery) {

  FILE *pFile;
  char plotId[32];
  char fName[94];
  char k[1];

  if((pFile = fopen(file_recovery->filename, "rb")) == NULL)
    return;

  if(fseek(pFile, 19, SEEK_SET)) {
    fclose(pFile);
    return;
  }

  if(fread(&plotId, 1, 32, pFile) <= 0) {
    fclose(pFile);
    return;
  }

  if(fread(&k, 1, 1, pFile) <= 0) {
    fclose(pFile);
    return;
  }
  fclose(pFile);

  int fptr = sprintf(fName, "plot-k");

  fptr += sprintf(fName+fptr, "%02d", k[0]);
  fptr += sprintf(fName+fptr, "-");

  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  fptr += sprintf(fName+fptr , "%04d", tm.tm_year + 1900);
  fptr += sprintf(fName+fptr, "-");
  fptr += sprintf(fName+fptr, "%02d", tm.tm_mon);
  fptr += sprintf(fName+fptr, "-");
  fptr += sprintf(fName+fptr, "%02d", tm.tm_mday);
  fptr += sprintf(fName+fptr, "-");
  fptr += sprintf(fName+fptr, "%02d", tm.tm_hour);
  fptr += sprintf(fName+fptr, "-");
  fptr += sprintf(fName+fptr, "%02d", tm.tm_min);
  fptr += sprintf(fName+fptr, "-");

  for (unsigned int i = 0; i < 32; i++)
    fptr += sprintf(fName+fptr, "%02X", plotId[i]);

  sprintf(fName+fptr, ".plot");

  for(int i = 0; fName[i]; i++)
    fName[i] = tolower(fName[i]);

  file_rename_unicode(file_recovery, fName, 94, 0, NULL, 1);
    return;
}

/*@
  @ requires separation: \separated(&file_hint_plot, buffer+(..), file_recovery, file_recovery_new);
  @ requires valid_header_check_param(buffer, buffer_size, safe_header_only, file_recovery, file_recovery_new);
  @ ensures  valid_header_check_result(\result, file_recovery_new);
  @ assigns  *file_recovery_new;
  @*/
static int header_check_plot(const unsigned char *buffer, const unsigned int buffer_size, const unsigned int safe_header_only, const file_recovery_t *file_recovery, file_recovery_t *file_recovery_new)
{
  reset_file_recovery(file_recovery_new);
  file_recovery_new->extension = file_hint_plot.extension;
  return 1;
}

static void register_header_check_plot(file_stat_t *file_stat)
{
  static const unsigned char plot_header[20] = {
    'P', 'r', 'o', 'o', 'f', 0x20, 'o', 'f', 0x20, 'S', 'p', 'a', 'c', 'e', 0x20, 'P', 'l', 'o', 't', 0x00
  };
  register_header_check(0, plot_header, sizeof(plot_header), &header_check_plot, file_stat);
}
#endif
