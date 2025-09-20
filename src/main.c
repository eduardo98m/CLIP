
#include <CLIP/List.h>
#include <string.h>


CLIP_DEFINE_LIST_TYPE(int)

// Helper for converting int → string
void int_to_str(int x, char *buf, size_t n) { 
  snprintf(buf, n, "%d", x); 
}

// Example of using a custom struct
typedef struct {
  const char *name;
  int age;
} Student;

void Student_to_str(Student x, char *buf, size_t n) {
  snprintf(buf, n, "{%s - %d}", x.name, x.age);
}

int compare_ints_ascending(const int *a, const int *b) {
    // A common and concise way to write the comparison
    return (*a > *b) - (*a < *b);
}

int compare_ints_descending(const int *a, const int *b) {
    // Just swap the logic for descending order
    return (*b > *a) - (*b < *a);
}

CLIP_DEFINE_LIST_TYPE(Student)

int main() {
  List(int) prices = List_init(int, 2);
  List_append(int, &prices, 20);
  List_append(int, &prices, 2);
  List_replace(int, &prices, 1, 5);

  char *s = List_to_str(int, &prices, int_to_str);
  printf("List contents: %s\n", s);
  free(s);
  List_free(int, &prices);

  // Lets create our group of Student

  Student Carlos = {.name = "Carlos", .age = 12};
  Student Marcos = {.name = "Marcos", .age = 13};
  Student Maria = {.name = "Maria", .age = 15};
  List(Student) classroom = List_init(Student,4);
  List_append(Student,&classroom, Carlos);
  List_append(Student,&classroom, Maria);
  List_append(Student,&classroom, Marcos);
  char *students = List_to_str(Student, &classroom, Student_to_str);

  printf("List of students :  %s \n", students);
  free(students);
  List_free(Student, &classroom);


  List(int) numbers = List_init(int, 8);

  // Append some unsorted values
  List_append(int, &numbers, 42);
  List_append(int, &numbers, -5);
  List_append(int, &numbers, 101);
  List_append(int, &numbers, 0);
  List_append(int, &numbers, 23);
  List_append(int, &numbers, -5); // Add a duplicate

  // Print the list before sorting
  char *before_str = List_to_str(int, &numbers, int_to_str);
  printf("Before sorting: %s\n", before_str);
  free(before_str);

  // 3. Call the new sort function with the ascending comparator
  List_sort(int, &numbers, compare_ints_ascending);

  // Print the list after sorting
  char *after_str_asc = List_to_str(int, &numbers, int_to_str);
  printf("After ascending sort:  %s\n", after_str_asc);
  free(after_str_asc);

  // Sort again, this time descending
  List_sort(int, &numbers, compare_ints_descending);
  char* after_str_desc = List_to_str(int, &numbers, int_to_str);
  printf("After descending sort: %s\n", after_str_desc);
  free(after_str_desc);

  // Clean up
  List_free(int, &numbers);

  return 0;
}
