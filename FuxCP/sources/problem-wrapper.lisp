(in-package :fuxcp)

(defun fux-cp-1st (counterpoint &optional (species 1))
    (let (
        (x (cffi::foreign-alloc :int :initial-contents counterpoint))
        )
        (first-sp x species)
    )
)

(cffi::defcfun ("first_sp" first-sp) :pointer
    (counterpoint :pointer :int)
    (species :int)
)