#ifndef HCQ_H
#define HCQ_H
#endif

/* Students are kept in order by time with newest
   students at the end of the lists. */
struct student{
    char *name;
    struct course *course;
    struct student *next_overall;
};

/* Tas are kept in reverse order of their time of addition. Newest
   Tas are kept at the head of the list. */
struct ta{
    char *name;
    struct student *current_student;
    struct ta *next;
};

struct course{
    char code[7];
};

typedef struct student Student;
typedef struct course Course;
typedef struct ta Ta;

/* TA management methods */
Ta *find_ta(Ta *ta_list, char *ta_name);
void add_ta(Ta **ta_list_ptr, char *ta_name);
int remove_ta(Ta **ta_list_ptr, char *ta_name);
int take_student(Ta *ta, Student **stu_list_ptr, Student *to_serve);
int next_overall(char *ta_name, Ta **ta_list_ptr, Student **stu_list_ptr);

/* Student management methods */
Student *find_student(Student *stu_list, char *student_name);
int add_student(Student **stu_list_ptr, char *student_name, char *course_num,
    Course *courses, int num_courses);
int give_up_waiting(Student **stu_list_ptr, char *student_name);

/* Statistics methods */
char *print_currently_serving(Ta *ta_list);
char *print_full_queue(Student *stu_list);

/* Error checking system call wrapper methods */
void *Malloc (size_t size);
int Read(int fd, void *buf, size_t nbytes);
int Write(int fd, const void *buf, size_t nbytes);
int Close(int fd);
