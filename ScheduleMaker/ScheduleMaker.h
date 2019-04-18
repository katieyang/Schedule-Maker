#pragma once
//The only change I made is between here ----------------------------

#include <iostream>
using namespace std;

#include <list>
#include <string>
#include <vector>
#include <unordered_map>

	class Event {
	public:
		string eventName;
		string eventDay;
		string eventStart;
		string eventEnd;

		__declspec(dllexport) Event(string name, string eDay, string eStart, string eEnd);

		bool operator==(const Event& second) {
			return this->eventName == second.eventName;
		}

	};

	class Course {
	public:
		string subject;
		string name;
		int year;
		int courseNumber;
		int priority; // default priority of 1
		vector<string> lectureTime; // includes the section depending on vector index
		vector<string> labTime;
		vector<string> tutorialTime;

		__declspec(dllexport) int getCourseNumber();

		__declspec(dllexport) Course(string sj, string nm, int yr, int crsNum, vector<string> lecture, vector<string> tut, vector<string> lab);

		bool operator==(const Course& second) {
			return this->name == second.name;
		}
	};

	class Courses {
	public:
		vector<string> subjects;
		vector<vector<int> > courseCodeMap;
		unordered_map<int, Course> courseClassMap;

		__declspec(dllexport) Courses(); //Read the file in, initialize all courses as objects, and then take prefix, concatenate with number of course and map that to the Course object

		__declspec(dllexport) Course searchSubj(const int key, unordered_map<int, Course> map);
	};

	class CourseException {
	private:
		string message;
	public:
		CourseException(const string&);
		string what() const;
	};

	class Day {
	public:
		unordered_map<string, string> times;
		__declspec(dllexport) Day();
	};

	class Schedule {
	public:
		vector<Day> schedule;

		__declspec(dllexport) Schedule();
		__declspec(dllexport) Schedule(const Schedule& right);
	};

	class ScheduleWrapper {
	public:
		vector<Course> enrolled;
		vector<Event> events;
		vector<Schedule> result;
		vector<Schedule> schedule_with_events;

		vector<string> course_conflicts;
		vector<string> event_conflicts;

		__declspec(dllexport) bool add_course(const Course& newCourse);
		__declspec(dllexport) void remove_course(const Course &removedCourse);
		__declspec(dllexport) bool check_placing(Schedule s, string d, vector<string> time_slots);
		__declspec(dllexport) bool can_place_lec(Course c, Schedule s, int time_slot);
		__declspec(dllexport) void place_course(Course c, Schedule& s, int time_slot);
		__declspec(dllexport) bool can_place_tut(Course c, Schedule& s, int time_slot);
		__declspec(dllexport) void place_tut(Course c, Schedule& s, int time_slot);
		__declspec(dllexport) bool can_place_lab(Course c, Schedule& s, int time_slot);
		__declspec(dllexport) void place_lab(Course c, Schedule& s, int time_slot);
		__declspec(dllexport) bool can_place_event(Event e, Schedule& s);
		__declspec(dllexport) void place_event(Event e, Schedule& s);
		__declspec(dllexport) void schedule_events();
		__declspec(dllexport) void optimize_morning();
		__declspec(dllexport) void helper_morning(int index, bool lecturePlaced, bool tutorialPlaced, bool labPlaced, Schedule curr);
		__declspec(dllexport) void optimize_evening();
		__declspec(dllexport) void helper_evening(int index, bool lecturePlaced, bool tutorialPlaced, bool labPlaced, Schedule curr);
		__declspec(dllexport) vector<string> split(string str, string sep);
		__declspec(dllexport) vector<string> get_time_slots(string start_time, string end_time);
		__declspec(dllexport) bool add_event(Event newEvent);
		__declspec(dllexport) void remove_event(Event removedEvent);
		__declspec(dllexport) vector<string> parse_times(string times);
		__declspec(dllexport) void showfirst();
	};