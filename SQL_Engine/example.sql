CREATE TABLE myTable (
    id INT,
    user VARCHAR
);
INSERT INTO myTable (id, user) VALUES (1, 'Alice'), (2, 'Bob'), (3, 'Charlie');
select user from myTable;
