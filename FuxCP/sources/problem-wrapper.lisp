(cl:defpackage "gil"
  (:nicknames "GIL")
  (:use common-lisp :cl-user :cl :cffi))

(in-package :gil)

(defun fux-cp-1st (counterpoint &optional (species 1))
    (let (
      (x (cffi::foreign-alloc :int :initial-contents counterpoint))
    )
    (1st-sp x species)
    )
)

(cffi::defcfun ("create_new_first_problem" 1st-sp) :pointer
    (counterpoint :pointer :int)
    (species :int)
)