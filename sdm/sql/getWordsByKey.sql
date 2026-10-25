select * from WORDS
where UPPER(w_word) like UPPER(?)
order by w_word