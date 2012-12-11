<?php
/*
 * Andrew Wessels 2012
 * Experimental use of Raphael JS library to create a timeline display of node data.
 * 6/28/2012 - STILL EXPERIMENTAL
 * 
 * Related files: /er/plugins/content_types/timeline.inc, /er/plugins/styles/timeline.inc,  /er/templates/er-timeline.tpl.php
 */

$chartID='chart-'.md5(rand());
$series_json = json_encode($series);
$info_json = json_encode($info);
//d($series);
//d($info);
?>
<h1><?=$info['title']?></h1>
<script type="text/javascript">
jQueryNew(document).ready(function($) {
	var blocks = <?=$series_json?>,
		settings = <?=$info_json?>,
		$stage = $("#chart-area-<?=$chartID?>"),
		timeline = new Timeline($stage, blocks, settings);
	//log($('ul.clear-block.ui-tabs-nav.tabs.primary'));
});
</script>

<div id="chart-area-<?=$chartID?>">
	<div class="positioner" style="position:absolute">
		<div id='tooltip'>
			<div class="title">Last, First</div>
			<div class="dates">January to March</div>
			<div class="body">Content</div>
		</div>
	</div>
	<div id="<?=$chartID?>" class="timeline" style="margin: 0 auto;<?=$info['style']?>"></div>
	<div class="chart-caption" style="text-align:right;"><?=$info['caption']?></div>
</div>