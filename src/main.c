
#include <CLIP/List.h>
#include <string.h>

// Helper for converting int → string

CLIP_DEFINE_LIST_TYPE(int)

void int_to_str(int x, char *buf, size_t n)
{
  snprintf(buf, n, "%d", x);
}

CLIP_REGISTER_LIST_PRINT(int, int_to_str)

int compare_ints_ascending(const int *a, const int *b)
{
  return (*a > *b) - (*a < *b);
}

int compare_ints_descending(const int *a, const int *b)
{
  return (*b > *a) - (*b < *a);
}

typedef struct
{
  const char *name;
  int age;
} Student;

void Student_to_str(Student x, char *buf, size_t n)
{
  snprintf(buf, n, "\n{%s - %d}", x.name, x.age);
}

CLIP_DEFINE_LIST_TYPE(Student)
CLIP_REGISTER_LIST_PRINT(Student, Student_to_str)

int main()
{
  // Lets create our group of Student

  Student Carlos = {.name = "Carlos", .age = 12};
  Student Marcos = {.name = "Marcos", .age = 13};
  Student Maria = {.name = "Maria", .age = 15};
  List(Student) classroom = List_init(Student, 4);
  List_append(Student, &classroom, Carlos);
  List_append(Student, &classroom, Maria);
  List_append(Student, &classroom, Marcos);
  char *students = List_to_str(Student, &classroom);

  printf("List of students :  %s \n", students);
  free(students);
  List_free(Student, &classroom);

  List(int) prices = List_init(int, 2);
  List_append(int, &prices, 20);
  List_append(int, &prices, 2);
  List_replace(int, &prices, 1, 5);

  char *s = List_to_str(int, &prices);
  printf("List contents: %s\n", s);
  free(s);
  List_free(int, &prices);

  List(int) numbers = List_init(int, 8);

  // Append some unsorted values
  List_append(int, &numbers, 42);
  List_append(int, &numbers, -5);
  List_append(int, &numbers, 101);
  List_append(int, &numbers, 0);
  List_append(int, &numbers, 23);
  List_append(int, &numbers, -5); // Add a duplicate

  List_foreach(const int, elem, &numbers)
  {
    double michael = *elem;
    printf("Value: %f\n", michael);
  }

  // Print the list before sorting
  char *before_str = List_to_str(int, &numbers);
  printf("Before sorting: %s\n", before_str);
  free(before_str);

  // 3. Call the new sort function with the ascending comparator
  List_sort(int, &numbers, compare_ints_ascending);

  // Print the list after sorting
  char *after_str_asc = List_to_str(int, &numbers);
  printf("After ascending sort:  %s\n", after_str_asc);
  free(after_str_asc);

  // Sort again, this time descending
  List_sort(int, &numbers, compare_ints_descending);
  char *after_str_desc = List_to_str(int, &numbers);
  printf("After descending sort: %s\n", after_str_desc);
  free(after_str_desc);

  // Clean up
  List_free(int, &numbers);

  Student raw_students[] = {
      {.name = "Alice", .age = 18},
      {.name = "Bob", .age = 19},
      {.name = "Charlie", .age = 20}};
  List(Student) group1 = List_init_from_static_array(Student, raw_students);

  printf("Group1 from static array:\n");
  List_println(Student, &group1);

  // === Another Student list ===
  Student more_students[] = {
      {.name = "Diana", .age = 21},
      {.name = "Eve", .age = 22}};
  List(Student) group2 = List_init_from_static_array(Student, more_students);

  char *group2_str = List_to_str(Student, &group2);
  printf("Group2 from static array: %s\n", group2_str);
  free(group2_str);

  // === Merge Student lists ===
  List_merge(Student, &group1, &group2);

  printf("Merged groups : \n ");
  List_println(Student, &group1);

  // Cleanup
  List_free(Student, &group1);
  List_free(Student, &group2);

  return 0;
}
