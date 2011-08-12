function AnimeRatings() {

	/// Redefine alert
	var _debug = true;
	this.alert = function(msg) {
		if (!confirm(msg + '\n\nPress Cancel to stop debugging.')) {
			_debug = false;
		}
	};
	window.alert = this.alert;


	this.onResponse = function(response) {
		if (response.success !== true) {
			throw "Invalid response: " + JSON.stringify(response);
		}

		if (response.action !== undefined && response.action !== null) {
			response.action();
		}
	};


	this.sendRequest = function(arg) {
		chrome.extension.sendRequest(arg, this.onResponse);
	};


	this.log = function(message) {
		this.sendRequest({ action: "log", arg: message }, this.onResponse);
	};
	window.log = this.log;

}


try {


var ar = new AnimeRatings();

ar.getMWPages = function() {
	var divs = document.getElementsByTagName("div");
	for (var i = 0; i < divs.length; ++i) {
		if (divs[i].id == "mw-pages") {
			return divs[i];
		}
	}
	throw "Could not find root node for the anime titles.";
};

ar.getLinksImpl = function(mwpages) {
	var result = [];
	var lis = mwpages.getElementsByTagName("li");
	for (var i = 0; i < lis.length; ++i) {
		var links = lis[i].getElementsByTagName("a");
		if (links.length > 0) {
			result.push(links[0]);
		}
	}
	if (result.length === 0) {
		throw "No page links were found.";
	}
	return result;
};


ar.getLinks = function(callback) {
	var linkNodes = this.getLinksImpl(this.getMWPages());
	for (var i = 0; i < linkNodes.length; ++i) {
		var linkNode = linkNodes[i];
		callback({title: linkNode.title, node: linkNode});
	}
};


//
// Application Entry Point
//
ar.getLinks(function(linkItem) {
	ar.log(linkItem.title);
});


} catch (exc) {
	alert(JSON.stringify(exc));
}

