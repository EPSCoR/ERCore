(function($){
	$(document).ready(function(){
		$('form#user-profile-form').bind('state:visible', function(e) {
			if (e.trigger) {
				$(e.target).closest('.form-item, .form-submit, .form-wrapper')[e.value ? 'fadeIn' : 'fadeOut']();
			}
		});
		//$('#er-help h2').click(function(){
		//	$(this).next().toggle();
		//});
		$('.help-tooltip').tooltip();
	});
})(jQuery);
