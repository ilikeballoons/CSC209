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
    if (stu_list == NULL) { // no added students
      return NULL;
    }

    Student *current_student = stu_list;
    while (current_student->next_overall) {
      if (strcmp(current_student->name, student_name) == 0) {
        return current_student;
      }
      current_student = current_student->next_overall;
    }
    return NULL;
}

/*   Return a pointer to the ta with name ta_name or NULL
 *   if no such TA exists in ta_list. TODO: test
 */
Ta *find_ta(Ta *ta_list, char *ta_name) {
  if (ta_list == NULL) {
    return NULL; // no TAs
  }
  while(strcmp(ta_list->name, ta_name) != 0) {
    if (ta_list->next != NULL) {
      ta_list = ta_list->next;
    } else {
      return NULL;
    }
  }
  return ta_list;
}

/*  Return a pointer to the course with this code in the course list
 *  or NULL if there is no course in the list with this code.
 */
Course *find_course(Course *courses, int num_courses, char *course_code) {
  if(courses == NULL) {
    return NULL; // no courses
  }
  for (int i = 0; i < num_courses; i++) {
    if (strcmp(courses[i].code, course_code) == 0) {
      return &courses[i];
    }
  }
    return NULL;
}

Student *new_student(char *name, Course *course) {
  Student *new = malloc(sizeof(Student));
  new->name = malloc(strlen(name) * sizeof(char));
  strcpy(new->name, name);

  new->arrival_time = malloc(sizeof(time_t));
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
int add_student(Student **stu_list_ptr, char *student_name, char *course_code,
    Course *course_array, int num_courses) {

    if (!(find_course(course_array, num_courses, course_code))) {
      return 2; // course not found
    }
    Course *found_course = find_course(course_array, num_courses, course_code);
    Student *new_student_loc = malloc(sizeof(Student));
    Student *student = new_student(student_name, found_course);
    memcpy(new_student_loc, student, sizeof(Student));

    if(!(*stu_list_ptr)) { // first student overall
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
int give_up_waiting(Student **stu_list_ptr, char *student_name) {
    return 0;
}

/* Create and prepend Ta with ta_name to the head of ta_list.
 * For the purposes of this assignment, assume that ta_name is unique
 * to the help centre and don't check it.
 */
void add_ta(Ta **ta_list_ptr, char *ta_name) {
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
void release_current_student(Ta *ta) {
  if (!ta->current_student) {
    return; //If the TA has no current student, do nothing.
  }

  Course *course = ta->current_student->course;
  course->helped++;

  time_t helped_time = time(NULL);
  // add the helped time to the course time
  // the value for this should be


}

/* Remove this Ta from the ta_list and free the associated memory with
 * both the Ta we are removing and the current student (if any).
 * Return 0 on success or 1 if this ta_name is not found in the list
 */
int remove_ta(Ta **ta_list_ptr, char *ta_name) {
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
int take_next_overall(char *ta_name, Ta *ta_list, Student **stu_list_ptr) {
    Ta *ta = find_ta(ta_list, ta_name);
    if (ta == NULL) { return 1; }

    // take the new student, record his arrival time
    // add NOW - arrival time to the student's course's wait_time
    // set the students arrival time to NOW

    // take the next overallstudent and set it to the TA's current students
    // also remove this student from the queue
    // increase the student's course's helped by 1

    //set the student's course's head to the next student in the courses

    ta->current_student = *stu_list_ptr;
    *stu_list_ptr = &(*stu_list_ptr[1]); //might not work
    time_t now = time(NULL);
    Course *course = ta->current_student->course;
    time_t waited_time = difftime(now, *(ta->current_student->arrival_time));
    course->wait_time += waited_time;
    *(ta->current_student->arrival_time) = now;

    course->helped++;
    course->head = course->head->next_course;

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
int take_next_course(char *ta_name, Ta *ta_list, Student **stu_list_ptr, char *course_code, Course *courses, int num_courses) {

    return 0;
}

/* For each course (in the same order as in the config file), print
 * the <course code>: <number of students waiting> "in queue\n" followed by
 * one line per student waiting with the format "\t%s\n" (tab name newline)
 * Uncomment and use the printf statements below. Only change the variable
 * names.
 */
void print_all_queues(Student *stu_list, Course *courses, int num_courses) {
         //printf("%s: %d in queue\n", var1, var2);
             //printf("\t%s\n",var3);
}

/*
 * Print to stdout, a list of each TA, who they are serving at from what course
 * Uncomment and use the printf statements
 */
void print_currently_serving(Ta *ta_list) {
    //printf("No TAs are in the help centre.\n");
    //printf("TA: %s is serving %s from %s\n",i var1, var2);
    //printf("TA: %s has no student\n", var3);
}

/*  list all students in queue (for testing and debugging)
 *   maybe suggest it is useful for debugging but not included in marking?
 */
void print_full_queue(Student *stu_list) {

}

/* Prints statistics to stdout for course with this course_code
 * See example output from assignment handout for formatting.
 *
 */
int stats_by_course(Student *stu_list, char *course_code, Course *courses, int num_courses, Ta *ta_list) {

    // TODO: students will complete these next pieces but not all of this
    //       function since we want to provide the formatting
    Course *found = find_course(courses, num_courses, course_code);
    printf("%s: %s \n", found->code, found->description);
    //extra functionality
    // Student *current_student = stu_list;
    // if(current_student->next_overall) {
    //   while (current_student->next_overall) {
    //     printf("%s: %s \n", (current_student->course)->code, current_student->name);
    //     printf("next_course: %s, overall: %s \n", (current_student->next_course)->name, (current_student->next_overall)->name);
    //     current_student = current_student->next_overall;
    //   }
    // }

    // You MUST not change the following statements or your code
    //  will fail the testing.
/*
    printf("%s:%s \n", found->code, found->description);
    printf("\t%d: waiting\n", students_waiting);
    printf("\t%d: being helped currently\n", students_being_helped);
    printf("\t%d: already helped\n", found->helped);
    printf("\t%d: gave_up\n", found->bailed);
    printf("\t%f: total time waiting\n", found->wait_time);
    printf("\t%f: total time helping\n", found->help_time);
*/
    return 0;
}

/* Helper function for creating new courses */
Course *new_course (char *course_code, char *course_desc) {
  Course *ptr = (Course*) malloc(sizeof(Course));
  strcpy(ptr->code, course_code);
  ptr->description = malloc(sizeof(char) * INPUT_BUFFER_SIZE);
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
      *courselist_ptr = malloc(course_num *sizeof(Course*));
      for (int i = 0; i < course_num; i++) { // read in the courses
        if (fgets(input_line, INPUT_BUFFER_SIZE, file) != NULL) { // error checking
          sscanf(input_line, "%s %[^\n]s", course_code, course_desc);
          (*courselist_ptr)[i] = *new_course(course_code, course_desc);
        }
      }
  }
  return course_num;
}
