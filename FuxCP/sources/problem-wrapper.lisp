(in-package :first)

(defun first-species ()
    (first-sp )
)

(cffi::defcfun ("first_sp" first-sp) :pointer

)

(defun solver ()
    (create-solver )
)

(cffi::defcfun ("create_solver" create-solver) :pointer

)

(defun next-sol ()
    (get-next )
)

(cffi::defcfun ("get_next" get-next) :pointer

)
