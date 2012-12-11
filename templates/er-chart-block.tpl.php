<?php
/*
 * Related files: /er/plugins/content_types/graph_pane.inc, /er/plugins/styles/pie_chart.inc, /er/templates/er-chart-block.tpl.php
 */
$chartID='chart-'.md5(rand());
$series_string = json_encode($series);
d($series_string);
?>
<script type="text/javascript">
	var chart;
	jQueryNew(document).ready(function($) {
		var maxlen = <?=isset($info['maxlen'])?$info['maxlen']:23?>; //chops off the tooltip text if it's too damn long.
		chart = new Highcharts.Chart({
			chart: {
				renderTo: '<?=$chartID?>',
				backgroundColor: null,
				plotBackgroundColor: null,
				plotBorderWidth: null,
				plotShadow: false
			},
			<?if ($info['title']):?>
			title: {
				text: '<?=$info['title']?>'
			},
			<?endif?>
			legend:{
				enabled: true
			},
			<?if ($info['subtitle']):?>
			subtitle: {
				text: '<?=$info['subtitle']?>'
			},
			<?endif?>
			<?if ($info['yAxis']):?>
			yAxis: <?=json_encode($info['yAxis'])?>,
			<?endif?>
			<?if ($info['xAxis']):?>
			xAxis: <?=json_encode($info['xAxis'])?>,
			<?endif?>
			tooltip: {
				formatter: function() {
					//var name = (this.point.name.length>maxlen?(this.point.name).substr(0, maxlen-3)+"...":this.point.name);
					//var name = (this.point.name.length>maxlen?this.point.name.substr(0,maxlen) + "<br>" + this.point.name.substr(maxlen):this.point.name);
					var name = this.point.name.replace(' or ', '/');
					return '<b>' + name + '</b><br>Count:' + this.y + '<br>' + (this.percentage).toFixed(2) + '%';
				}
			},
			plotOptions: {
				pie: {
					allowPointSelect: true,
					cursor: 'pointer',
					dataLabels: {
						enabled: true,
						distance: 12,
						color: '#000000',
						connectorColor: '#000000',
						formatter: function() {
							return Math.round(this.percentage) + '%';
							//return '<b>'+ this.point.name +'</b>: '+ (this.percentage).toFixed(2)+' %';
						}
					}//,
            		//showInLegend: true
				}
			},
		    series: <?=$series_string?>
		});
	});
</script>
<div class="chart-area">
	<div id="<?=$chartID?>" class="chart" style="max-width: 550px; max-height: 500px; margin: 0 auto;<?=$info['style']?>"></div>
	<div class="chart-caption" style="text-align:right;"><?=$info['caption']?></div>
</div>