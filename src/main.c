
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

  return 0;
}
