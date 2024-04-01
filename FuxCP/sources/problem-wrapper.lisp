(in-package :first)

(defun first-species (counterpoint &optional (species 1))
    (first-sp counterpoint species)
)

(cffi::defcfun ("first_sp" first-sp) :pointer
    (counterpoint :int)
    (species :int)
)