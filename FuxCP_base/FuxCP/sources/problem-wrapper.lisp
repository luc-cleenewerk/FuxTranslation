(cl:defpackage "gilf"
  (:nicknames "GILF")
   (:use common-lisp :cl-user :cl :cffi))

(in-package :gilf)
(in-package :fuxcp)

(print "Loading gecode-wrapper...")

(defparameter DFS 0)
(defparameter BAB 1)
; corresponds to enum values in gecode_problem.h, but can be used graphically in om
(defun bab ()
    BAB
)
(defun dfs ()
    DFS
)

;;;;;;;;;;;;;;;;;;;;;
;; Problem methods ;;
;;;;;;;;;;;;;;;;;;;;;


;; This function is used to convert lisp lists into int pointers so they can be passed to c++
(defun new-ctp-problem (lb ub sp cf pc mtc spl con obl dir var)
    (let (
        (x (cffi::foreign-alloc :int :initial-contents cf))
        (y (cffi::foreign-alloc :int :initial-contents spl))
    )
    (new-problem (length cf) lb ub sp x pc mtc y con obl dir var)
    )
)

(cffi::defcfun ("create_new_problem" new-problem) :pointer
    "Creates a new instance of the problem. Returns a void* cast of a Problem*."
    (size               :int) ; an integer representing the size
    (lower-bound-domain :int) ; an integer representing the lower bound of the domain
    (upper-bound-domain :int) ; an integer representing the upper bound of the domain
    (species            :int)
    (cantus-firmus      :pointer :int)
    (pcost              :int)
    (mtricost           :int)
    (species-list       :pointer :int)
    (con-motion-cost    :int)
    (obl-motion-cost    :int)
    (dir-motion-cost    :int)
    (variety-cost       :int)
    ; TODO add here any additional arguments that your Problem constructor takes
)

(cffi::defcfun ("get_size" get-size) :int
    "Returns the size of the space."
    (sp :pointer) ; a void* cast of a Problem*
)

(cffi::defcfun ("test_cffi" test-cffi) :int
    "Returns n+1."
    (n :int) ; an integer
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Search engine methods ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;

(cffi::defcfun ("create_solver" create-solver) :pointer
    "Creates a DFS<Problem> object. Returns a void* cast of a DFS<Problem> object."
    (sp :pointer) ; a void* cast of a Problem*
    (solver-type :int); an integer representing the type of the solver (see above)
)

(cffi::defcfun ("return_next_solution_space" return-next-solution-space) :pointer
    "Returns a pointer to the next solution of the problem. Returns a void* cast of a Problem*."
    (solver :pointer) ; a void* cast of a Base<Problem>* pointer
)

;;;;;;;;;;;;;;;;;;;;;;;
;; Solution handling ;;
;;;;;;;;;;;;;;;;;;;;;;;

(cffi::defcfun ("return_solution" return-solution) :pointer
    "Returns a int* that are the values of a solution."
        (sp :pointer) ; a void* cast of a Problem object that is a solution of the problem.
)

(defun solution-to-int-array (sp)
    "Returns the values the variables have taken in the solution as a list of integers. Casts a int* into a list of numbers."
    "sp is a void* cast of a Problem* that is a solution to the problem. Calling this funciton on a non-solution 
        will result in an error."
        (if (cffi::null-pointer-p sp) ; TODO check
            (error "No (more) solutions.")
        )
    (let* (
            (size (get-size sp))
            (ptr (return-solution sp))
        )
        (loop for i from 0 below size
            collect (cffi::mem-aref ptr :int i)
        )
    )
)


(print "finished loading gecode-wrapper")