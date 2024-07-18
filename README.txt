ΑΡΧΕΙΑ:
	4 c source file 2 για το memory segment και reader/writer
	1 αρχειο header
	makefile
	README
MAKEFILE:
	make για την δημιουργια εκτελεσιμων ./reader ./writer ./memory ./readmemory
	make clean για διαγραφη εκτελεσιμων
ΠΑΡΑΤΗΡΗΣΕΙΣ:
	./memory για την δημιουργια του shared memory και αρχικοποιηση semaphore
	./reader με τα ζητουμενα flags για να διαβαστει ενα η παραπανω records 
	./writer με τα ζητουμενα flags για την αλλαγη του balance
	./readmemory <id> για την τυπωση του shared memory και την αποδεσμευση semaphores
	Ο συγχρονισμος reader/writer δεν ειναι σωστός. Υπάρχει μόνο ενας semaphore που ομως λειτουργεί σωστά. Επιτρέπει μόνο ενα
	process στο memory segment. Δεν προλαβα να υλοποιήσω περεταίρω λόγω χρόνου.