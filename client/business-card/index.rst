.. _pmixexamples:client:business-card:

Business Card Exchange
======================

This example shows the use of Client interfaces that are used
during the bootstrap phase of the parallel application, where
the *business card* connection information of the processes is exchanged.

Setup
-----

* Prereq: OpenPMIx library and PMIx server (e.g., PRRTE)

Build/install PMIx library and server
(e.g., `OpenPMIx <https://github.com/openpmix/openpmix>`_ and
`PRRTE <https://github.com/openpmix/prrte>`_)

Compile and run example (e.g., using prrte+openpmix)

.. code-block:: shell

    cd client/business-card/src/
    make
    prterun -np 4 ./ipv4_bc_use_case


Walk-Through
------------

Highlevel walk-through of code example...

.. code-block:: C
   :emphasize-lines: 2,5

    void c_hello(){
        printf("Hello World!\n");
    }

    int main() {
        c_hello();
        return 0;
    }

This is text after.

Full Source Listing
-------------------

.. note:: We may want to remove this but including for completeness and to
   have an example in case you want to have full listing.

.. literalinclude:: src/ipv4_bc_use_case.c
   :language: c
   :linenos:
   :caption: Business card exchange example 'ipv4_bc_use_case.c'

Sources
-------
 - :download:`Makefile <src/Makefile>`
 - :download:`Download the source <src/ipv4_bc_use_case.c>`
 - KenRs GitHub Gist ipv4_bc_use_case.c `<https://gist.githubusercontent.com/raffenet/9b41d212b610c18b61e6fb4dd3973acc/raw/ad636d8cbd2f2293420cd992fc75393630810797/ipv4_bc_use_case.c>`_

