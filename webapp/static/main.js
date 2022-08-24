$(document).ready(function() {
  Plotly.newPlot(
    'plotly_chart',
    [current_data],
    layout
  );
});



// update with new data every second
// https://stackoverflow.com/a/2170924/7418735
var intervalId = window.setInterval(function(){
  fetchData();
}, 1000);


function fetchData() {

  $.ajax(
    {
      url: "/ajax",
      type: "GET"
    }
  )
    .done(function(data) {

      // overwrite data object to save memory
      current_data = data;

      // https://plotly.com/javascript/streaming/#streaming-with-timestamp
      let update = {
        x: [current_data['x']],
        y: [current_data['y']]
      }

      Plotly.extendTraces(
        'plotly_chart',
        update,
        [0]
      );
    });
}


// plotly chart layout
var layout = {
  xaxis: {
    title: "Time point",
    tick0: 0
  },
  yaxis: {
    // TODO: kph toggle
    title: "MPH",
    tick0: 0
  },
  hovermode: "x",
  line: {shape: 'spline'},
  title: "Bike performance"
};
