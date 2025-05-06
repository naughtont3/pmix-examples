# PMIx Examples

Repository of examples for PMIx use cases.

The content is written in ReStructured Text (RST) format and
uses the Sphinx package to format the document.


Building Docs Manually
----------------------

 * Step 0: FIRST TIME ONLY

   ```
   shell$ python3 -m venv venv
   shell$ source venv/bin/activate
   shell$ pip install -r requirements.txt
   ```


 * Step 1: Standard usage for edit and build

   ```
   shell$ source venv/bin/activate
   (venv) shell$ make
   (venv) shell$ vi myfile.rst
   (venv) shell$ make
   ```

 *  Step 2: View HTML results (e.g., using ``open`` utility on macOS)

   ```
   (venv) shell$ open _build/index.html
   ```


ReadTheDocs Note
----------------
 - Created account at ReadTheDocs.org
 - Imported this public Github project
    - https://readthedocs.org/dashboard/
    - https://readthedocs.org/projects/PMIx Examples/
 - Added `.readthedocs.yaml` to Github project
 - RTD does the rest
 - URL: https://pmix-examples.readthedocs.io/en/latest/

