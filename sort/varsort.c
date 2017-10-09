#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "sort.h"
#include <ctype.h>

int column;

int compare(const void *p1, const void *p2) {
  unsigned int num1;
  unsigned int num2;
  if (column > (((rec_dataptr_t *)p1) -> data_ints) - 1)
    num1 = (((rec_dataptr_t *)p1) -> data_ptr)
[((rec_dataptr_t *)p1) -> data_ints - 1];
  else
    num1 = (((rec_dataptr_t *)p1) -> data_ptr)[column];
  if (column > (((rec_dataptr_t *)p2) -> data_ints) - 1)
    num2 = (((rec_dataptr_t *)p2) -> data_ptr)
[((rec_dataptr_t *)p2) -> data_ints - 1];
  else
    num2 = (((rec_dataptr_t *)p2) -> data_ptr)[column];
  return(num1 > num2);
}

// print out the error if there is no argument
void usage(char *prog) {
  fprintf(stderr,
  "Usage: ./varsort -i inputfile -o outputfile [-c column]\n");
  exit(1);
}

int main(int argc, char *argv[]) {
  // read file
  char *inFile = NULL;
  char *outFile = NULL;
  int c;
  while ((c = getopt(argc, argv, "i:o:c:")) != -1) {
    switch (c) {
      case 'i':
        inFile = strdup(optarg);
        break;
      case 'o':
        outFile = strdup(optarg);
        break;
      case 'c':
        column = atoi(optarg);
        if (column < 0) {
          fprintf(stderr,
          "Error: Column should be a non-negative integer\n");
          exit(1);
        }
        break;
      default:
        usage(argv[0]);
    }
  }

  if (inFile == NULL || outFile == NULL) {
    usage(argv[0]);
  }

  // open input file
  int fd = open(inFile, O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "Error: Cannot open file %s\n", inFile);
    exit(1);
  }

  // get the number of record
  int recordsLeft;
  int rc;
  rc = read(fd, &recordsLeft, sizeof(recordsLeft));
  if (rc != sizeof(recordsLeft)) {
    fprintf(stderr, "read() failed\n");
    exit(1);
  }
  // printf("Number of records: %d\n", recordsLeft);

  // get the index num and size of data
  rec_nodata_t r;
  rec_dataptr_t *records = malloc(sizeof(rec_dataptr_t) * recordsLeft);

  for (int i = 0; i < recordsLeft; i++) {
    // store index and size information in records
    rc = read(fd, &r, sizeof(rec_nodata_t));
    if (rc != sizeof(rec_nodata_t)) {
      perror("read the index and size of data failed\n");
      exit(1);
    }
    (records + i) -> index = r.index;
    (records + i) -> data_ints = r.data_ints;

    // store datas in records
    unsigned int *data_ptr = malloc(sizeof(unsigned int) * r.data_ints);
    rc = read(fd, data_ptr, sizeof(unsigned int) * r.data_ints);
    if (rc != sizeof(unsigned int) * r.data_ints) {
      perror("read data failed\n");
      exit(1);
    }
    (records + i) -> data_ptr = data_ptr;
  }

  qsort(records, recordsLeft, sizeof(rec_dataptr_t), compare);

  // Open and create output file.
  int fd1 = open(outFile, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
  if (fd1 < 0) {
     fprintf(stderr, "Error: Cannot open file %s\n", outFile);
     exit(1);
  }

  // Output the number of records.
  rc = write (fd1, &recordsLeft, sizeof(recordsLeft));
  for (int i = 0; i < recordsLeft; i++) {
    unsigned int index = records[i].index;
    rc = write(fd1, &index, sizeof(unsigned int));
    if (rc != sizeof(unsigned int)) {
      perror("write record failed\n");
      exit(1);
    }
    unsigned int size = records[i].data_ints;
    rc = write(fd1, &size, sizeof(unsigned int));
    if (rc != sizeof(unsigned int)) {
      perror("write size of record failed\n");
      exit(1);
    }
    rc = write(fd1, records[i].data_ptr,
sizeof(unsigned int) * records[i].data_ints);
    if (rc != sizeof(unsigned int) * records[i].data_ints) {
      perror("write data failed\n");
      exit(1);
    }
  }
  for (int i = 0; i < recordsLeft; i++) {
    free((records + i) -> data_ptr);
  }
  free(records);
  (void) close(fd);
  (void) close(fd1);
  free(inFile);
  free(outFile);
  return 0;
}
