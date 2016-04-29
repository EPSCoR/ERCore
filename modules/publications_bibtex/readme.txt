Instructions
For this BibTeX sub-module to function correctly in ERCore it requires:
- In the ERcore's Publications View (er_summary_er_publication) create a new XML export display (display machine name should be:'views_data_export_xml') . 
- Make sure the format of this new display 'views_data_export_xml' is exactly what is in Kia's screenshots: https://goo.gl/photos/G51gzhscQzCJ6Pfx8
- After above step is completed, then put this file (publications_bibtex.module) and the publications_bibtex.info file under folder er/modules/publications_bibtex, and then go to Drupal modules and enable this module. The BibTex Converter will show up under EPSCoR Admin Views menu.

Attention: 
1) ERCore content type Publication may not return the correct author order therefore **YOU MUST VERIFY** the order of the authors after you upload this BibTeX file to Research.gov to make sure the author order matches exactly to what is on the actual publication.

2) BibTex is only generated for the following type of publications: Journal Article, Book and Conference Proceedings
