CREATE TABLE student(sid integer, name char(32), grade char(4), gpa double);
CREATE TABLE classes(cid integer, classname char(20), enrollment integer, sid integer);
INSERT INTO classes(cid, classname, enrollment, sid) 
	VALUES (1, 'Databases', 200, 4);
INSERT INTO classes (sid, cid, classname, enrollment) 
	VALUES (10, 2, 'Program', 150);
SELECT classes.cid FROM classes WHERE classes.cid > classes.sid;
DROP TABLE student;
DROP TABLE classes;