#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hcq.h"
#define INPUT_BUFFER_SIZE 256
#define COURSE_CODE_SIZE 7

/*
 * Return a pointer to the struct student with name stu_name
 * or NULL if no student with this name exists in the stu_list
 */
Student *find_student(Student *stu_list, char *student_name) {
    if (stu_list == NULL) { return NULL; }

    Student *current_student = stu_list;
    while (current_student->next_overall) {
      if (strcmp(current_student->name, student_name) == 0) { return current_student; }
      current_student = current_student->next_overall;
    }
    return NULL;
}

/*   Return a pointer to the ta with name ta_name or NULL
 *   if no such TA exists in ta_list.
 */
Ta *find_ta (Ta *ta_list, char *ta_name) {
  if (ta_list == NULL) { return NULL; } // no TAs
  Ta *ta = ta_list;
  while (strcmp(ta->name, ta_name) != 0) {
    if (ta->next != NULL) { ta = ta->next; }
    else { return NULL; }
  }
  return ta;
}

/*  Return a pointer to the course with this code in the course list
 *  or NULL if there is no course in the list with this code.
 */
Course *find_course (Course *courses, int num_courses, char *course_code) {
  if (courses == NULL) {
    return NULL; // no courses
  }
  for (int i = 0; i < num_courses; i++) {
    if (strcmp(courses[i].code, course_code) == 0) {
      return &courses[i];
    }
  }
    return NULL;
}

void delete_student (Student *student) {
  free(student->arrival_time);
  free(student->name);
  free(student);
}

Student *new_student(char *name, Course *course) {
  Student *new = malloc(sizeof(Student));
  if (new == NULL) {
     perror("malloc for new");
     exit(1);
  }
  new->name = malloc(strlen(name));
  if (new->name == NULL) {
     perror("malloc for new->name");
     exit(1);
  }
  strcpy(new->name, name);

  new->arrival_time = malloc(sizeof(time_t));
  if (new->arrival_time == NULL) {
     perror("malloc for new->arrival_time");
     exit(1);
  }
  *(new->arrival_time) = time(NULL);
  new->course = course;
  new->next_overall = NULL;
  new->next_course = NULL;

  return new;
}

/* Add a student to the queue with student_name and a question about course_code.
 * TODO: if a student with this name already has a question in the queue (for any
   course), return 1 and do not create the student.
 * If course_code does not exist in the list, return 2 and do not create
 * the student struct.
 * For the purposes of this assignment, don't check anything about the
 * uniqueness of the name.
 */
int add_student (Student **stu_list_ptr, char *student_name, char *course_code,
    Course *course_array, int num_courses) {

    if (!(find_course(course_array, num_courses, course_code))) {
      return 2; // course not found
    }
    Course *found_course = find_course(course_array, num_courses, course_code);
    Student *new_student_loc = malloc(sizeof(Student));
    if (new_student_loc == NULL) {
       perror("malloc for new_student_loc");
       exit(1);
    }
    Student *student = new_student(student_name, found_course);
    memcpy(new_student_loc, student, sizeof(Student));

    if (!(*stu_list_ptr)) { // first student overall
      *stu_list_ptr = new_student_loc;
    } else { // other students in the queue
      // if (find_student(*stu_list_ptr, student_name)) {
      //   return 1; // student exists in queue NOTE doesn't work properly
      // }
      /*
      * Need to add this new student into 1, possibly two locations
      * student_queue_tail->next_overall (always)
      *if during the queue traversal the found course is found, then a new traversal should begin
      * this traversal goes along the next_course and adds a pointer to this student there as well
      */

      Student *student_queue_tail = *stu_list_ptr;
      while (student_queue_tail->next_overall) { // traverse the queue
        student_queue_tail = student_queue_tail->next_overall;
      }
      student_queue_tail->next_overall = new_student_loc;
    }
    if (!(found_course->head)) {// if there are no students in this course
      // make this student the head and the tail
      found_course->head = new_student_loc;
      found_course->tail = new_student_loc;

    } else {
      (found_course->tail)->next_course = new_student_loc;
      found_course->tail = new_student_loc;
    }
    return 0;
}
/* Student student_name has given up waiting and left the help centre
 * before being called by a Ta. Record the appropriate statistics, remove
 * the student from the queues and clean up any no-longer-needed memory.
 *
 * If there is no student by this name in the stu_list, return 1.
 */
int give_up_waiting (Student **stu_list_ptr, char *student_name) {
  //find the student
  Student *found_student = find_student(*stu_list_ptr, student_name);
  // if not found return 1
  if (found_student == NULL) { return 1; }

  // add the students now - arrival time to the courses waited time
  time_t now = time(NULL);
  found_student->course->wait_time += difftime(now, *(found_student->arrival_time));
  // increase the courses->bailed by 1
  found_student->course->bailed++;

  //find the student in the queue whose next_overall is this student
  Student *queued_overall_student = *stu_list_ptr;
  while (queued_overall_student && queued_overall_student != found_student) {
    queued_overall_student = queued_overall_student->next_overall;
  }
  //find the student in the queue whose next_course is this student
  Student *queued_course_student = found_student->course->head;
  if (found_student != found_student->course->head) {
    //traverse the found student's course head->next_course until the next student is the found students
    while (queued_course_student && queued_course_student != found_student) {
      queued_course_student = queued_course_student->next_course;
    }
  }
  // check if these two found students are the same
  if (queued_course_student == queued_overall_student) {
    //if yes (students are in the same course):
    // set the next_overall to the found student's next_overall (can be null)
    queued_course_student->next_overall = found_student->next_overall;
    // set the next_course to the found student's next_course (can also be null)
    queued_course_student->next_course = found_student->next_course;

  } else {
    //if no (students are in different courses):
    //set the next_overall to the found student's next_overall (can be null)
    queued_overall_student->next_overall = found_student->next_overall;
    // set this second found student (the one whose next course is the found student) to the found_student's next_course (can be null)
    queued_course_student->next_course = found_student->next_course;
  }
    return 0;
}

/* Create and prepend Ta with ta_name to the head of ta_list.
 * For the purposes of this assignment, assume that ta_name is unique
 * to the help centre and don't check it.
 */
void add_ta (Ta **ta_list_ptr, char *ta_name) {
    // first create the new Ta struct and populate
    Ta *new_ta = malloc(sizeof(Ta));
    if (new_ta == NULL) {
       perror("malloc for TA");
       exit(1);
    }
    new_ta->name = malloc(strlen(ta_name)+1);
    if (new_ta->name  == NULL) {
       perror("malloc for TA name");
       exit(1);
    }
    strcpy(new_ta->name, ta_name);
    new_ta->current_student = NULL;

    // insert into front of list
    new_ta->next = *ta_list_ptr;
    *ta_list_ptr = new_ta;
}

/* The TA ta is done with their current student.
 * Calculate the stats (the times etc.) and then
 * free the memory for the student.
 * If the TA has no current student, do nothing.
 */
void release_current_student (Ta *ta) {
  if (!ta->current_student) {
    return; //If the TA has no current student, do nothing.
  }
  time_t now = time(NULL);
  Course *course = ta->current_student->course;
  time_t helped_time = difftime(now, *(ta->current_student->arrival_time));
  course->help_time += helped_time;
  course->helped++;

  delete_student(ta->current_student);
}

/* Remove this Ta from the ta_list and free the associated memory with
 * both the Ta we are removing and the current student (if any).
 * Return 0 on success or 1 if this ta_name is not found in the list
 */
int remove_ta (Ta **ta_list_ptr, char *ta_name) {
    Ta *head = *ta_list_ptr;
    if (head == NULL) {
        return 1;
    } else if (strcmp(head->name, ta_name) == 0) {
        // TA is at the head so special case
        *ta_list_ptr = head->next;
        release_current_student(head);
        // memory for the student has been freed. Now free memory for the TA.
        free(head->name);
        free(head);
        return 0;
    }
    while (head->next != NULL) {
        if (strcmp(head->next->name, ta_name) == 0) {
            Ta *ta_tofree = head->next;
            //  We have found the ta to remove, but before we do that
            //  we need to finish with the student and free the student.
            //  You need to complete this helper function
            release_current_student(ta_tofree);

            head->next = head->next->next;
            // memory for the student has been freed. Now free memory for the TA.
            free(ta_tofree->name);
            free(ta_tofree);
            return 0;
        }
        head = head->next;
    }
    // if we reach here, the ta_name was not in the list
    return 1;
}

/* TA ta_name is finished with the student they are currently helping (if any)
 * and are assigned to the next student in the full queue.
 * If the queue is empty, then TA ta_name simply finishes with the student
 * they are currently helping, records appropriate statistics,
 * and sets current_student for this TA to NULL.
 * If ta_name is not in ta_list, return 1 and do nothing.
 */
int take_next_overall (char *ta_name, Ta *ta_list, Student **stu_list_ptr) {
    Ta *ta = find_ta(ta_list, ta_name);
    if (!ta) { return 1; }

    //set the student's course's head to the next student in the courses
    release_current_student(ta);
    if (!(*stu_list_ptr)) { // if there are no students in the queue
      ta->current_student = NULL;
      return 0;
    }
    ta->current_student = *stu_list_ptr;
    Course *course = ta->current_student->course;
    time_t now = time(NULL);
    course->wait_time += difftime(now, *(ta->current_student->arrival_time));
    *(ta->current_student->arrival_time) = now;

    *stu_list_ptr = (*stu_list_ptr)->next_overall;
    if (course->head == course->tail) {
      course->head = NULL;
      course->tail = NULL;
    } else {
      course->head = course->head->next_course;
    }
    return 0;
}

/* TA ta_name is finished with the student they are currently helping (if any)
 * and are assigned to the next student in the course with this course_code.
 * If no student is waiting for this course, then TA ta_name simply finishes
 * with the student they are currently helping, records appropriate statistics,
 * and sets current_student for this TA to NULL.
 * If ta_name is not in ta_list, return 1 and do nothing.
 * If course is invalid return 2, but finish with any current student.
 */
int take_next_course (char *ta_name, Ta *ta_list, Student **stu_list_ptr, char *course_code, Course *courses, int num_courses) {
  Ta *ta = find_ta(ta_list, ta_name);
  if (ta == NULL) { return 1; }
  Course *course = find_course(courses, num_courses, course_code);
  release_current_student(ta);
  if (!course) { // invalid course
    ta->current_student = NULL;
    return 2;
  }

  if (!course->head) { // 0 students waiting
    release_current_student(ta);
    return 0;
  }

  ta->current_student = course->head;
  if (course->head == course->tail) { // 1 student waiting
    course->head = NULL;
    course->tail = NULL;
  } else { // 2+ students waiting
    course->head = course->head->next_course;
  }
  if (ta->current_student == *stu_list_ptr) { // student happens to be at front of queue anyway
    *stu_list_ptr = (*stu_list_ptr)->next_overall;
  } else { // if the student is not at the head of the student queue, we need to remove him from the queue and link the student that was pointing to him to his next_overall
    Student *previous_student = *stu_list_ptr;
    while (previous_student->next_overall && previous_student->next_overall != ta->current_student) {
      previous_student = previous_student->next_overall;
    }
    // this should now be the student whose next_overall is the current student
    previous_student->next_overall = ta->current_student->next_overall;
  }

  time_t now = time(NULL);
  course->wait_time += difftime(now, *(ta->current_student->arrival_time));
  *(ta->current_student->arrival_time) = now;

  return 0;
}

/* For each course (in the same order as in the config file), print
 * the <course code>: <number of students waiting> "in queue\n" followed by
 * one line per student waiting with the format "\t%s\n" (tab name newline)
 * Uncomment and use the printf statements below. Only change the variable
 * names.
 */
void print_all_queues (Student *stu_list, Course *courses, int num_courses) {
  if (num_courses < 1) { return; }
  for (int i = 0; i < num_courses; i++) {
    int waiting_students = 0;
    Student *head = courses[i].head;
    while (head) {
      waiting_students++;
      head = head->next_course;
    }
    printf("%s: %d in queue\n", courses[i].code, waiting_students);
    head = courses[i].head;
    while (head) {
      printf("\t%s\n", head->name);
      head = head->next_course;
    }
  }
}

/*
 * Print to stdout, a list of each TA, who they are serving at from what course
 * Uncomment and use the printf statements
 */
void print_currently_serving(Ta *ta_list) {
  if (!ta_list) {
    printf("No TAs are in the help centre.\n");
    return;
  }
  Ta *ta = ta_list;
  while(ta){
    char *ta_name = ta->name;
    if (ta->current_student) {
     char *student_name = ta->current_student->name;
     char *code = ta->current_student->course->code;
     printf("TA: %s is serving %s from %s\n", ta_name, student_name, code);
    } else { printf("TA: %s has no student\n", ta_name); }
    ta = ta->next;
  }
}

/*  list all students in queue (for testing and debugging)
 *   maybe suggest it is useful for debugging but not included in marking?
 */
void print_full_queue(Student *stu_list) {
  Student *current_student = stu_list;
  if (!current_student) { return; } // no students
  while(current_student) {
    printf("Name: %s\tCourse: %s\tArrival Time: %s\n",
     current_student->name, current_student->course->code,
     asctime(gmtime(current_student->arrival_time)));
    if (current_student->next_overall) {
      if (current_student->next_course && current_student->next_overall) {
        printf("Next Overall: %s\t Next Course: %s\n",
        current_student->next_overall->name, current_student->next_course->name);
      } else {
        printf("Next Overall: %s\t Next Course: NULL\n",
        current_student->next_overall->name);
      }
      current_student = current_student->next_overall;
    } else {
      current_student = NULL;
    }
  }
}

/* Prints statistics to stdout for course with this course_code
 * See example output from assignment handout for formatting.
 *
 */
int stats_by_course(Student *stu_list, char *course_code, Course *courses, int num_courses, Ta *ta_list) {
    Course *found = find_course(courses, num_courses, course_code);
    Student *course_student = found->head;

    int students_waiting = 0;
    while (course_student) { // check that this is correct TODO
      students_waiting++;
      if (course_student->next_course) { course_student = course_student->next_course; } //TODO: CHECK!!!!!
      else { course_student = NULL; }
    }

    Ta *ta = ta_list;
    int students_being_helped = 0;
    while (ta) {
      if(ta->current_student) {
        if (ta->current_student->course == found) { students_being_helped++; }
      }
      if (ta->next) { ta = ta->next; }
      else { ta = NULL; }
    }

    // You MUST not change the following statements or your code
    //  will fail the testing.

    printf("%s:%s \n", found->code, found->description);
    printf("\t%d: waiting\n", students_waiting);
    printf("\t%d: being helped currently\n", students_being_helped);
    printf("\t%d: already helped\n", found->helped);
    printf("\t%d: gave_up\n", found->bailed);
    printf("\t%f: total time waiting\n", found->wait_time);
    printf("\t%f: total time helping\n", found->help_time);
    return 0;
}

/* Helper function for creating new courses */
Course *new_course (char *course_code, char *course_desc) {
  Course *ptr = (Course*) malloc(sizeof(Course));
  if (ptr == NULL) {
     perror("malloc for course ptr");
     exit(1);
  }
  strcpy(ptr->code, course_code);
  ptr->description = malloc(INPUT_BUFFER_SIZE);
  if (ptr->description == NULL) {
     perror("malloc for ptr->description");
     exit(1);
  }
  strcpy(ptr->description, course_desc);
  ptr->head = NULL;
  ptr->tail = NULL;
  ptr->helped = 0;
  ptr->bailed = 0;
  ptr->wait_time = 0.0;
  ptr->help_time = 0.0;
  return ptr;
}

/* Dynamically allocate space for the array course list and populate it
 * according to information in the configuration file config_filename
 * Return the number of courses in the array.
 * If the configuration file can not be opened, call perror() and exit.
 */
int config_course_list(Course **courselist_ptr, char *config_filename) {
  int course_num = 0;
  FILE *file;
  file = fopen(config_filename, "r");

  char input_line[INPUT_BUFFER_SIZE];
  char course_code[COURSE_CODE_SIZE];
  char course_desc[INPUT_BUFFER_SIZE-COURSE_CODE_SIZE];

  if (file == NULL) {
    perror("Error opening file");
    exit(1);
  }
  if (fgets(input_line, INPUT_BUFFER_SIZE, file) != NULL) { // set the course number from the first line of file
      sscanf(input_line, "%d", &course_num);
      *courselist_ptr = malloc(course_num * sizeof(Course));
      if (courselist_ptr == NULL) {
         perror("malloc for courselist_ptr");
         exit(1);
      }
      for (int i = 0; i < course_num; i++) { // read in the courses
        if (fgets(input_line, INPUT_BUFFER_SIZE, file) != NULL) { // error checking
          sscanf(input_line, "%s %[^\n]s", course_code, course_desc);
          (*courselist_ptr)[i] = *new_course(course_code, course_desc);
        }
      }
  }
  return course_num;
}
