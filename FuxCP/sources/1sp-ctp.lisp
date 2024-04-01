(in-package :fuxcp)

; Author: Thibault Wafflard, adapted by Anton Lamotte
; Date: June 3, 2023, adapted January 2024
; This file contains the function that adds all the necessary constraints to the first species.

;;==========================#
;; FIRST SPECIES            #
;;==========================#
(defun fux-cp-1st (counterpoint &optional (species 1))
    (print "########## FIRST SPECIES ##########")
    "Create the CSP for the first species of Fux's counterpoint."

    ;============================================ CREATING GIL ARRAYS =============================
    ;; initialize the variables
    (print "Initializing variables...")

    ; creating harmonic intervals array
    (print "Creating harmonic intervals array...")

    ; array of IntVar representing the absolute intervals % 12 between the cantus firmus and the counterpoint
    (setf (first (h-intervals counterpoint)) (gil::add-int-var-array *sp* *cf-len 0 11))
    (create-h-intervals (first (notes counterpoint)) (first (notes *lowest)) (first (h-intervals counterpoint)))

    ; creating melodic intervals array
    (print "Creating melodic intervals array...")
    ; array of IntVar representing the absolute intervals between two notes in a row of the counterpoint
    (setf (first (m-intervals counterpoint)) (gil::add-int-var-array *sp* *cf-last-index 0 12))
    
    #| next line defined in init-counterpoint |#
    ; (setf (first (m-intervals-brut counterpoint)) (gil::add-int-var-array *sp* *cf-last-index -12 12))
    (create-m-intervals-self (first (notes counterpoint)) (first (m-intervals counterpoint)) (first (m-intervals-brut counterpoint)))
    
    (case species ((1 3v-1st) ; only for the first species
        ; then
        (progn
            ; creating melodic intervals array between the note n and n+2
            (setf (m2-intervals counterpoint) (gil::add-int-var-array *sp* *cf-penult-index 0 12))
            (setf (m2-intervals-brut counterpoint) (gil::add-int-var-array *sp* *cf-penult-index -12 12))
            (create-m2-intervals (first (notes counterpoint)) (m2-intervals counterpoint) (m2-intervals-brut counterpoint))
            
            ; creating boolean is counterpoint off key array
            (print "Creating is counterpoint off key array...")
            (setf (is-cp-off-key-arr counterpoint) (gil::add-bool-var-array *sp* *cf-len 0 1))
            (create-is-member-arr (first (notes counterpoint)) (is-cp-off-key-arr counterpoint) (off-domain counterpoint))
        )
    ))

    ; creating perfect consonances boolean array
    (print "Creating perfect consonances boolean array...")
    ; array of BoolVar representing if the interval between the cantus firmus and the counterpoint is a perfect consonance    
    (setf (is-p-cons-arr counterpoint) (gil::add-bool-var-array *sp* *cf-len 0 1))
    (create-is-p-cons-arr (first (h-intervals counterpoint)) (is-p-cons-arr counterpoint))


    ; creating order/role of pitch array (if cantus firmus is higher or lower than counterpoint)
    ; 0 for being the bass, 1 for being above
    (print "Creating order of pitch array...")
    (setf (first (is-cf-lower-arr counterpoint)) (gil::add-bool-var-array *sp* *cf-len 0 1))
    (create-is-cf-lower-arr (first (notes counterpoint)) *cf (first (is-cf-lower-arr counterpoint)))


    ; creating motion array
    (print "Creating motion array...")
    (setf (first (motions counterpoint)) (gil::add-int-var-array *sp* *cf-last-index -1 2)) ; 0 = contrary, 1 = oblique, 2 = direct/parallel
    (setf (first (motions-cost counterpoint)) (gil::add-int-var-array-dom *sp* *cf-last-index *motions-domain*))
    (create-motions (first (m-intervals-brut counterpoint)) (first (m-intervals-brut *lowest)) (first (motions counterpoint)) (first (motions-cost counterpoint)) (is-not-lowest counterpoint))

    
    ;============================================ HARMONIC CONSTRAINTS ============================
    
    (first::first-species counterpoint species)

    ;============================================ MELODIC CONSTRAINTS =============================
    
    ;============================================ COST FACTORS ====================================
    (print "Cost function...")

    

    (setf (solution-array counterpoint) (first (notes counterpoint)))
    (setf (solution-len counterpoint) *cf-len)

    ; RETURN
    (case species 
        (1 (append (fux-search-engine (solution-array counterpoint)) (list (list 1))))
        (otherwise nil) ; if 3v don't return a search engine, just apply the constraints
    )
)