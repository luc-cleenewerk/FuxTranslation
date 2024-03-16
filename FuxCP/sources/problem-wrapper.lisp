(cl:defpackage "gil"
  (:nicknames "GIL")
  (:use common-lisp :cl-user :cl :cffi))

(in-package :gil)

(cffi::defcfun ("fux_cp_1st" fux-cp-1st) :void
    (counterpoint :pointer)
    (species :int)
)

(cffi::defcfun ("create_h_intervals" create-h-intervals) :void
    (cp :pointer)
    (lowest :pointer)
    (h-intervals :pointer)
)