var TabSetup = {

    init: function() {
	this.initTabbedBlocks();
    },

    initTabbedBlocks: function() {
	// set up tabbed code blocks
	$('.tab-content').find('.tab-pane').each(function(idx, item) {
            var navTabs = $(this).closest('.code-tabs').find('.nav-tabs'),
		title = $(this).attr('title');
            navTabs.append('<li><a href="#">'+title+'</a></li');
	});

	$('.code-tabs ul.nav-tabs').each(function() {
            $(this).find("li:first").addClass('active');
	})

	$('.code-tabs .tab-content').each(function() {
            // $(item).find('tab-pane').addClass('active');
            $(this).find("div:first").addClass('active');
	});

	$('.nav-tabs a').click(function(e){
            e.preventDefault();
            var tab  = $(this).parent(),
		tabIndex = tab.index(),
		tabPanel = $(this).closest('.code-tabs'),
		tabPane = tabPanel.find('.tab-pane').eq(tabIndex);
            tabPanel.find('.active').removeClass('active');
            tab.addClass('active');
            tabPane.addClass('active');
	});

	// todo - optimize and make less terrible
	$('.code-tabs').each(function() {
            var largest = 0;
            var codeHeight = 0;
            var panes = $(this).find('.tab-pane');
            panes.each(function() {
		var outerHeight = $(this).outerHeight();
		console.log("outerHeight: " + outerHeight);
		if (outerHeight > largest) {
		    largest = outerHeight;
		    codeHeight = $(this).find('code').outerHeight();
		}
            });
            console.log("codeHeight: " + codeHeight);
            panes.each(function() {
		$(this).height(largest);
		// make all the <code> elements the same height to
		// avoid it jumping around when switching tabs
		$(this).find('code').height(largest - 5);
            });
	});
    }

};

$(document).ready(function() {
    TabSetup.init();
});
