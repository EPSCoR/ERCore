jQuery(document).ready(function(){
	jQuery('form#user-profile-form').bind('state:visible', function(e) {
	  if (e.trigger) {
	    jQuery(e.target).closest('.form-item, .form-submit, .form-wrapper')[e.value ? 'fadeIn' : 'fadeOut']();
	  }
	});	
})